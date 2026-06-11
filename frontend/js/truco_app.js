const CAMINHO_SONS = "/assets/sons/";
const sonsPreCarregados = {};
const parametros = new URLSearchParams(window.location.search);
const idSala = parametros.get("sala");

let socket = null;
let meuId = null;
let meuNome = "";
let equipeEscolhida = 1;
let estadoAtual = null;
let estadoAnterior = null;
let reconectarAutomaticamente = true;
let tentativaReconexao = 0;
let intervaloPing = null;
let eventoAnterior = "";
let novaMaoAgendada = false;
let historicoQuedas = { 1: undefined, 2: undefined, 3: undefined };

const modalEntrada = document.getElementById("modal-entrada");
const modalEspera = document.getElementById("modal-espera");
const inputNome = document.getElementById("input-nome");
const btnEntrarJogo = document.getElementById("btn-entrar-jogo");
const erroEntrada = document.getElementById("erro-entrada");
const botoesEquipe = document.querySelectorAll(".btn-equipe");

document.addEventListener("DOMContentLoaded", iniciar);

function iniciar() {
    if (!idSala) {
        alert("Código da sala de Truco não informado.");
        window.location.href = "/pages/truco_config.html";
        return;
    }

    ["jogar_carta.ogg", "shuffle.mp3", "victory_6.mp3", "click.mp3"].forEach(preCarregarSom);

    document.getElementById("codigo-sala-entrada").textContent = `Sala ${idSala}`;

    inputNome.value = localStorage.getItem(chaveNome()) || "";
    equipeEscolhida = Number(localStorage.getItem(chaveEquipe()) || 1);
    atualizarSelecaoEquipe();

    botoesEquipe.forEach(botao => {
        botao.addEventListener("click", () => {
            equipeEscolhida = Number(botao.dataset.equipe);
            atualizarSelecaoEquipe();
        });
    });

    btnEntrarJogo.addEventListener("click", entrarNaSala);
    inputNome.addEventListener("keydown", evento => {
        if (evento.key === "Enter") entrarNaSala();
    });

    conectarWebSocket();
}

function preCarregarSom(nomeArquivo) {
    const audio = new Audio(`${CAMINHO_SONS}${nomeArquivo}`);
    audio.preload = "auto";
    audio.load();
    sonsPreCarregados[nomeArquivo] = audio;
}

function tocarSom(nomeArquivo) {
    const original = sonsPreCarregados[nomeArquivo];
    if (!original) return;

    const audio = original.cloneNode(true);
    audio.currentTime = 0;
    audio.play().catch(() => {});
}

function chaveToken() { return `truco_token_${idSala}`; }
function chaveNome() { return `jogador_nickname`; }
function chaveEquipe() { return `truco_equipe_preferida`; }
function chaveSessaoAtiva() { return `truco_sessao_${idSala}`; }

function obterTokenReconexao() {
    let token = localStorage.getItem(chaveToken());
    if (!token) {
        token = typeof crypto !== "undefined" && crypto.randomUUID
            ? crypto.randomUUID()
            : `${Date.now()}-${Math.random().toString(36).slice(2)}-${Math.random().toString(36).slice(2)}`;
        localStorage.setItem(chaveToken(), token);
    }
    return token;
}

function atualizarSelecaoEquipe() {
    botoesEquipe.forEach(botao => {
        botao.classList.toggle("selecionado", Number(botao.dataset.equipe) === equipeEscolhida);
    });
}

function conectarWebSocket() {
    atualizarIndicadorConexao("Conectando ao servidor", "Aguarde...");

    const protocolo = window.location.protocol === "https:" ? "wss:" : "ws:";
    socket = new WebSocket(`${protocolo}//${window.location.host}/ws/truco`);

    socket.onopen = () => {
        tentativaReconexao = 0;
        iniciarPing();
        atualizarIndicadorConexao("Conectado ao servidor", `Sala ${idSala}`);

        if (localStorage.getItem(chaveSessaoAtiva()) === "true") {
            enviarEntrada(true);
        } else {
            abrirModalEntrada();
        }
    };

    socket.onmessage = evento => {
        let dados;
        try {
            dados = JSON.parse(evento.data);
        } catch {
            console.error("Mensagem inválida recebida:", evento.data);
            return;
        }
        processarMensagem(dados);
    };

    socket.onerror = erro => {
        console.error("Erro no WebSocket do Truco:", erro);
    };

    socket.onclose = () => {
        pararPing();
        atualizarIndicadorConexao("Conexão interrompida", "Tentando reconectar...");

        if (!reconectarAutomaticamente) return;

        const atraso = Math.min(1000 * (2 ** tentativaReconexao), 10000);
        ++tentativaReconexao;
        setTimeout(conectarWebSocket, atraso);
    };
}

function iniciarPing() {
    pararPing();
    intervaloPing = setInterval(() => { enviarMensagem({ tipo: "ping" }); }, 20000);
}

function pararPing() {
    if (!intervaloPing) return;
    clearInterval(intervaloPing);
    intervaloPing = null;
}

function processarMensagem(dados) {
    switch (dados.tipo) {
        case "conectado":
        case "pong":
            return;

        case "entrou_sala":
        case "reconectado_sala":
            meuId = dados.id_jogador;
            localStorage.setItem(chaveSessaoAtiva(), "true");
            fecharModalEntrada();
            if (!dados.partida_iniciada) abrirModalEspera();
            return;

        case "estado_jogo":
            atualizarInterface(dados);
            return;

        case "sessao_substituida":
            reconectarAutomaticamente = false;
            mostrarModal("<h2>Sessão substituída</h2><p>Esta sala foi aberta em outra janela.</p>", 0);
            return;

        case "erro":
            tratarErro(dados.mensagem || "Erro desconhecido.");
            return;
    }
}

function tratarErro(mensagem) {
    console.error("Erro do Truco:", mensagem);
    const erroDeEntrada = !meuId || mensagem.includes("equipe") || mensagem.includes("sala") || mensagem.includes("entrar") || mensagem.includes("cheia");

    if (erroDeEntrada) {
        reconectarAutomaticamente = false; 
        localStorage.removeItem(chaveSessaoAtiva());
        abrirModalEntrada();
        erroEntrada.textContent = mensagem;
        btnEntrarJogo.disabled = false;
        btnEntrarJogo.textContent = "Entrar";
        return;
    }
    mostrarModal(`<h2>Não foi possível realizar a ação</h2><p>${mensagem}</p>`, 2200);
}

function entrarNaSala() {
    const nome = inputNome.value.trim();

    if (nome.length < 2) {
        erroEntrada.textContent = "Digite um nome com pelo menos dois caracteres.";
        inputNome.focus();
        return;
    }

    if (!socket || socket.readyState !== WebSocket.OPEN) {
        erroEntrada.textContent = "A conexão com o servidor ainda não está pronta.";
        return;
    }

    meuNome = nome;
    localStorage.setItem(chaveNome(), meuNome);
    localStorage.setItem(chaveEquipe(), String(equipeEscolhida));

    erroEntrada.textContent = "";
    btnEntrarJogo.disabled = true;
    btnEntrarJogo.textContent = "Entrando...";

    enviarEntrada(false);
}

function enviarEntrada(reconexao) {
    const mensagem = {
        tipo: "entrar_sala",
        sala: idSala,
        token: obterTokenReconexao()
    };

    if (!reconexao) {
        mensagem.nome = meuNome || inputNome.value.trim();
        mensagem.equipe = equipeEscolhida;
    }
    enviarMensagem(mensagem);
}

function enviarMensagem(mensagem) {
    if (!socket || socket.readyState !== WebSocket.OPEN) return false;
    socket.send(JSON.stringify(mensagem));
    return true;
}

function abrirModalEntrada() {
    modalEspera.classList.add("modal-oculto");
    modalEntrada.classList.remove("modal-oculto");
    setTimeout(() => inputNome.focus(), 100);
}

function fecharModalEntrada() {
    modalEntrada.classList.add("modal-oculto");
    btnEntrarJogo.disabled = false;
    btnEntrarJogo.textContent = "Entrar";
    erroEntrada.textContent = "";
}

function abrirModalEspera() {
    modalEspera.classList.remove("modal-oculto");
}

function fecharModalEspera() {
    modalEspera.classList.add("modal-oculto");
}

function atualizarInterface(dados) {
    estadoAnterior = estadoAtual;
    estadoAtual = dados;
    meuId = dados.meu_id;

    if (!dados.partida_iniciada) {
        abrirModalEspera();
        document.getElementById("info-jogadores-espera").textContent = `${dados.jogadores_registrados} de ${dados.max_jogadores} jogadores na sala.`;
        atualizarIndicadorConexao("Aguardando jogadores", `${dados.jogadores_registrados}/${dados.max_jogadores} jogadores`);
        renderizarJogadores(dados);
        return;
    }

    fecharModalEntrada();
    fecharModalEspera();

    const novaDistribuicao = verificarNovaDistribuicao(dados);

    if (dados.ultimo_evento === "NOVA_MAO" || (dados.numero_queda === 1 && (!dados.jogadas_queda || dados.jogadas_queda.length === 0))) {
        historicoQuedas = { 1: undefined, 2: undefined, 3: undefined };
        renderizarPainelQuedas();
    }

    atualizarPlacar(dados);
    atualizarInformacoesMao(dados);
    renderizarVira(dados.vira);
    renderizarCartasDaQueda(dados.jogadas_queda || []);
    renderizarJogadores(dados);
    renderizarMinhaMao(dados, novaDistribuicao);
    atualizarPainelAcoes(dados);
    atualizarIndicadorTurno(dados);
    tratarEvento(dados);
}

function verificarNovaDistribuicao(dados) {
    if (!estadoAnterior) return false;
    const quantidadeAnterior = estadoAnterior.minha_mao?.length || 0;
    const quantidadeAtual = dados.minha_mao?.length || 0;

    if (quantidadeAtual > quantidadeAnterior && quantidadeAtual === 3) {
        animarDistribuicao(dados);
        return true;
    }
    return false;
}

function atualizarPlacar(dados) {
    document.getElementById("pontos-eq1").textContent = dados.pontos_equipe_1 ?? 0;
    document.getElementById("pontos-eq2").textContent = dados.pontos_equipe_2 ?? 0;

    const equipe1 = dados.jogadores?.find(jogador => jogador.equipe === 1);
    const equipe2 = dados.jogadores?.find(jogador => jogador.equipe === 2);

    if (dados.max_jogadores === 2) {
        document.getElementById("nome-equipe-1").textContent = equipe1?.nome || "Equipe 1";
        document.getElementById("nome-equipe-2").textContent = equipe2?.nome || "Equipe 2";
    } else {
        document.getElementById("nome-equipe-1").textContent = "Equipe 1";
        document.getElementById("nome-equipe-2").textContent = "Equipe 2";
    }
}

function atualizarInformacoesMao(dados) {
    const valorMao = document.getElementById("valor-mao");
    valorMao.textContent = dados.valor_mao ?? 1;
    valorMao.classList.toggle("mao-de-onze", Boolean(dados.mao_de_onze));
    document.getElementById("queda-atual").textContent = `${dados.numero_queda || 1}ª queda`;
}

function renderizarVira(vira) {
    const elemento = document.getElementById("carta-vira");
    if (!vira || vira.valor === undefined) {
        elemento.innerHTML = "?";
        elemento.className = "carta carta-vazia";
        elemento.style.backgroundImage = "none";
        return;
    }
    preencherCarta(elemento, vira);
}

function renderizarCartasDaQueda(jogadas) {
    const mesa = document.getElementById("cartas-na-mesa");
    mesa.innerHTML = "";

    jogadas.forEach((jogada, indice) => {
        if (!jogada.carta) return;
        const carta = document.createElement("div");
        preencherCarta(carta, jogada.carta);
        carta.title = nomeDoJogador(jogada.id_jogador);
        carta.style.setProperty("--rotacao-mesa", `${[-5, 4, -2, 6][indice % 4]}deg`);
        mesa.appendChild(carta);
    });
}

function renderizarJogadores(dados) {
    ["cadeira-esquerda", "cadeira-topo", "cadeira-direita"].forEach(id => {
        document.getElementById(id).innerHTML = "";
    });

    const jogadores = dados.jogadores || [];
    const total = jogadores.length;

    jogadores.forEach(jogador => {
        if (jogador.id === meuId) return;

        const cadeira = obterCadeira(jogador.id, total);
        if (!cadeira) return;

        const suaVez = dados.jogador_atual === jogador.id;
        const cartas = Array.from(
            { length: jogador.quantidade_cartas || 0 },
            () => '<div class="carta-verso"></div>'
        ).join("");

        document.getElementById(cadeira).innerHTML = `
            <div class="perfil-jogador ${suaVez ? "sua-vez" : ""}">
                <h3>${escaparHtml(jogador.nome || `Jogador ${jogador.id + 1}`)}</h3>
                ${dados.max_jogadores === 4 ? `<span class="tag-equipe tag-equipe-${jogador.equipe}">Equipe ${jogador.equipe}</span>` : ""}
                ${!jogador.conectado ? '<span class="status-desconectado">Desconectado</span>' : ""}
            </div>
            <div class="mao-oponente">${cartas}</div>
        `;
    });

    const jogadorLocal = jogadores.find(jogador => jogador.id === meuId);
    if (jogadorLocal) {
        meuNome = jogadorLocal.nome;
        document.getElementById("nome-local").textContent = jogadorLocal.nome;
        document.getElementById("local-equipe").textContent = jogadorLocal.equipe;
        document.getElementById("local-quedas").textContent = jogadorLocal.equipe === 1 ? dados.vitorias_equipe_1 || 0 : dados.vitorias_equipe_2 || 0;
        document.querySelector(".dados-local").classList.toggle("dados-local-ocultos", dados.max_jogadores === 2);
    }
}

function renderizarMinhaMao(dados, ocultarTemporariamente = false) {
    const mao = document.getElementById("minha-mao");
    const cartas = dados.minha_mao || [];
    const meio = (cartas.length - 1) / 2;
    mao.innerHTML = "";

    cartas.forEach((carta, indice) => {
        const elemento = document.createElement("div");
        preencherCarta(elemento, carta);
        elemento.style.setProperty("--rotacao", `${(indice - meio) * 6}deg`);
        elemento.style.setProperty("--transY", `${Math.abs(indice - meio) * 4}px`);

        if (ocultarTemporariamente) {
            elemento.style.opacity = "0";
            elemento.style.pointerEvents = "none";
            setTimeout(() => {
                elemento.style.transition = "opacity 0.3s";
                elemento.style.opacity = "1";
                elemento.style.pointerEvents = "auto";
            }, 850);
        }

        if (dados.pode_jogar) {
            elemento.addEventListener("click", () => jogarCarta(indice));
        } else {
            elemento.classList.add("bloqueada");
        }

        mao.appendChild(elemento);
    });
}

function atualizarPainelAcoes(dados) {
    const painel = document.getElementById("painel-acoes");
    const titulo = document.getElementById("titulo-acao");
    const botoes = document.getElementById("botoes-acao");

    botoes.innerHTML = "";
    painel.classList.add("escondido");

    if (dados.fase === "MAO_FINALIZADA") {
        painel.classList.remove("escondido");
        titulo.textContent = "Mão encerrada:";
        botoes.appendChild(criarBotao("Nova mão", "aceitar", iniciarNovaMao));
        return;
    }

    if (dados.pode_responder_truco) {
        painel.classList.remove("escondido");
        titulo.textContent = `${dados.nome_pedidor || "Adversário"} pediu ${nomeValorTruco(dados.valor_pedido)}:`;
        botoes.appendChild(criarBotao("Aceitar", "aceitar", aceitarTruco));
        botoes.appendChild(criarBotao("Recusar", "recusar", recusarTruco));
        if (proximoValor(dados.valor_pedido) !== 0) {
            botoes.appendChild(criarBotao(`${nomeValorTruco(proximoValor(dados.valor_pedido))}!`, "aumentar", aumentarTruco));
        }
        return;
    }

    if (dados.pode_pedir_truco) {
        painel.classList.remove("escondido");
        titulo.textContent = "Sua vez:";
        botoes.appendChild(criarBotao(`${nomeValorTruco(proximoValor(dados.valor_mao))}!`, "truco", pedirTruco));
    }
}

function atualizarIndicadorTurno(dados) {
    const indicador = document.getElementById("indicador-turno");
    const texto = document.getElementById("indicador-turno-texto");
    const fase = document.getElementById("indicador-turno-fase");

    indicador.className = "indicador-turno";

    if (dados.fase === "PARTIDA_FINALIZADA") {
        indicador.classList.add("finalizado");
        texto.textContent = `${nomeEquipe(dados.vencedor_partida)} venceu`;
        fase.textContent = "Partida finalizada";
        return;
    }

    if (dados.fase === "MAO_FINALIZADA") {
        indicador.classList.add("aguardando");
        texto.textContent = dados.mensagem || "Mão encerrada";
        fase.textContent = "Aguardando próxima mão";
        return;
    }

    if (dados.pode_responder_truco) {
        indicador.classList.add("alerta");
        texto.textContent = "Sua equipe deve responder";
        fase.textContent = `Pedido de ${nomeValorTruco(dados.valor_pedido)}`;
        return;
    }

    if (dados.jogador_atual === meuId) {
        indicador.classList.add("minha-vez");
        texto.textContent = "É a sua vez";
        fase.textContent = `${dados.numero_queda}ª queda`;
        return;
    }

    indicador.classList.add("aguardando");
    const jogadorAtual = dados.jogadores?.find(jogador => jogador.id === dados.jogador_atual);
    texto.textContent = `Vez de ${jogadorAtual?.nome || "outro jogador"}`;
    fase.textContent = `${dados.numero_queda}ª queda`;
}

function atualizarIndicadorConexao(texto, fase) {
    const indicador = document.getElementById("indicador-turno");
    indicador.className = "indicador-turno aguardando";
    document.getElementById("indicador-turno-texto").textContent = texto;
    document.getElementById("indicador-turno-fase").textContent = fase;
}

function tratarEvento(dados) {
    const assinatura = [
        dados.ultimo_evento,
        dados.mensagem,
        dados.pontos_equipe_1,
        dados.pontos_equipe_2,
        dados.numero_queda,
        dados.valor_pedido
    ].join("|");

    if (!dados.ultimo_evento || assinatura === eventoAnterior) return;
    eventoAnterior = assinatura;

    switch (dados.ultimo_evento) {
        case "FIM_QUEDA":
            // CORREÇÃO: Como o servidor já avançou o número da rodada, 
            // a queda que acabou de ser vencida é a anterior (- 1).
            historicoQuedas[dados.numero_queda - 1] = dados.vencedor_ultima_queda;
            renderizarPainelQuedas();

            let msgQueda = "";
            if (dados.vencedor_ultima_queda === 0) {
                msgQueda = "A queda empatou!";
            } else {
                msgQueda = `${nomeEquipe(dados.vencedor_ultima_queda)} levou a queda!`;
            }

            mostrarToastQueda(msgQueda, dados.vencedor_ultima_queda);
            bloquearInteracaoTemporariamente(2500);
            break;

        case "TRUCO_PEDIDO":
            mostrarModal(`<h2 class="texto-dourado">${escaparHtml(dados.nome_pedidor || "Um jogador")} pediu ${nomeValorTruco(dados.valor_pedido)}!</h2><p>A mão passará a valer ${dados.valor_pedido} pontos se for aceita.</p>`, 2200);
            tocarSom("click.mp3");
            break;

        case "TRUCO_ACEITO":
            mostrarModal(`<h2 class="texto-verde">Pedido aceito</h2><p>A mão agora vale ${dados.valor_mao} pontos.</p>`, 2000);
            break;

        case "TRUCO_RECUSADO":
            mostrarModal(`<h2 class="texto-vermelho">Pedido recusado</h2><p>${escaparHtml(dados.mensagem || "")}</p>`, 2200);
            break;

        case "FIM_MAO":
            // No FIM_MAO, o servidor NÃO avança a rodada, então usamos o numero_queda normal
            if (dados.vencedor_ultima_queda !== undefined && dados.numero_queda !== undefined) {
                historicoQuedas[dados.numero_queda] = dados.vencedor_ultima_queda;
                renderizarPainelQuedas();
                
                const msgQuedaMao = dados.vencedor_ultima_queda === 0 ? "A última queda empatou!" : `${nomeEquipe(dados.vencedor_ultima_queda)} levou a última queda!`;
                mostrarToastQueda(msgQuedaMao, dados.vencedor_ultima_queda);
            }

            mostrarModal(`<h2 class="texto-dourado">Fim da mão</h2><p>${escaparHtml(dados.mensagem || "")}</p>`, 2300);
            tocarSom("victory_6.mp3");
            break;

        case "FIM_PARTIDA":
            reconectarAutomaticamente = false;
            localStorage.removeItem(chaveToken());
            localStorage.removeItem(chaveSessaoAtiva());

            mostrarModal(
                `<h2 class="texto-dourado">${nomeEquipe(dados.vencedor_partida)} venceu!</h2>
                 <p>Placar final: ${dados.pontos_equipe_1} × ${dados.pontos_equipe_2}</p>
                 <a class="link-modal" href="/pages/menu.html">Voltar ao menu</a>`,
                0
            );
            tocarSom("victory_6.mp3");
            break;
    }
}

function jogarCarta(indice) {
    tocarSom("jogar_carta.ogg");
    enviarMensagem({ tipo: "acao_jogo", acao: "JOGAR_CARTA", indice });
}

function pedirTruco() {
    tocarSom("click.mp3");
    enviarMensagem({ tipo: "acao_jogo", acao: "PEDIR_TRUCO" });
}

function aceitarTruco() {
    tocarSom("click.mp3");
    enviarMensagem({ tipo: "acao_jogo", acao: "ACEITAR_TRUCO" });
}

function recusarTruco() {
    tocarSom("click.mp3");
    enviarMensagem({ tipo: "acao_jogo", acao: "RECUSAR_TRUCO" });
}

function aumentarTruco() {
    tocarSom("click.mp3");
    enviarMensagem({ tipo: "acao_jogo", acao: "AUMENTAR_TRUCO" });
}

function iniciarNovaMao() {
    if (novaMaoAgendada) return;
    novaMaoAgendada = true;
    enviarMensagem({ tipo: "acao_jogo", acao: "NOVA_MAO" });
    setTimeout(() => { novaMaoAgendada = false; }, 1000);
}

function criarBotao(texto, classe, acao) {
    const botao = document.createElement("button");
    botao.className = `btn-acao ${classe}`;
    botao.textContent = texto;
    botao.addEventListener("click", acao);
    return botao;
}

// --- LÓGICA DOS SPRITES ---

function obterNomeArquivoSprite(valor, naipe) {
    const mapaNaipes = {
        "paus": "clubs",
        "ouros": "diamonds",
        "copas": "hearts",
        "espadas": "spades"
    };

    const mapaValores = {
        1: "ace", 2: "02", 3: "03", 4: "04", 5: "05",
        6: "06", 7: "07", 8: "08", 9: "09", 10: "10",
        11: "jack", 12: "queen", 13: "king"
    };

    const spriteNaipe = mapaNaipes[naipe];
    const spriteValor = mapaValores[Number(valor)];

    if (!spriteNaipe || !spriteValor) return "back03.png";

    return `${spriteNaipe}_${spriteValor}.png`;
}

function preencherCarta(elemento, carta) {
    const naipeNormalizado = normalizarNaipe(carta.naipe);
    const nomeArquivo = obterNomeArquivoSprite(carta.valor, naipeNormalizado);

    elemento.className = "carta";
    elemento.innerHTML = ""; 
    elemento.style.backgroundImage = `url('../assets/cartas/${nomeArquivo}')`;
}

function normalizarNaipe(naipe) {
    if (typeof naipe === "string") {
        const normalizado = naipe.toLowerCase();
        if (normalizado.includes("ouro")) return "ouros";
        if (normalizado.includes("copa")) return "copas";
        if (normalizado.includes("espada")) return "espadas";
        if (normalizado.includes("pau")) return "paus";
        return normalizado;
    }
    const mapaNumerico = { 0: "ouros", 1: "copas", 2: "espadas", 3: "paus" };
    return mapaNumerico[Number(naipe)] || "";
}

function obterCadeira(idJogador, totalJogadores) {
    const posicao = (idJogador - meuId + totalJogadores) % totalJogadores;
    if (totalJogadores === 2) return "cadeira-topo";
    if (posicao === 1) return "cadeira-direita";
    if (posicao === 2) return "cadeira-topo";
    if (posicao === 3) return "cadeira-esquerda";
    return "";
}

function nomeDoJogador(idJogador) {
    return estadoAtual?.jogadores?.find(jogador => jogador.id === idJogador)?.nome || `Jogador ${idJogador + 1}`;
}

function nomeEquipe(equipe) {
    if (!estadoAtual || estadoAtual.max_jogadores !== 2) {
        return `Equipe ${equipe}`;
    }
    return estadoAtual.jogadores?.find(jogador => jogador.equipe === equipe)?.nome || `Equipe ${equipe}`;
}

function nomeValorTruco(valor) {
    return { 3: "Truco", 6: "Seis", 9: "Nove", 12: "Doze" }[Number(valor)] || `${valor} pontos`;
}

function proximoValor(valor) {
    return { 1: 3, 3: 6, 6: 9, 9: 12 }[Number(valor)] || 0;
}

function mostrarModal(conteudo, tempo) {
    const modal = document.getElementById("modal-notificacao");
    const texto = document.getElementById("modal-texto");
    texto.innerHTML = conteudo;
    modal.classList.remove("modal-oculto");
    clearTimeout(window.temporizadorModal);
    if (tempo > 0) {
        window.temporizadorModal = setTimeout(() => { modal.classList.add("modal-oculto"); }, tempo);
    }
}

function escaparHtml(texto) {
    const elemento = document.createElement("div");
    elemento.textContent = String(texto ?? "");
    return elemento.innerHTML;
}

function animarDistribuicao(dados) {
    tocarSom("shuffle.mp3");
    const centro = document.querySelector(".centro-da-mesa");
    if (!centro || !dados.jogadores) return;

    const origem = centro.getBoundingClientRect();
    let atraso = 0;

    dados.jogadores.forEach(jogador => {
        const cadeira = jogador.id === meuId ? "cadeira-base" : obterCadeira(jogador.id, dados.jogadores.length);
        const alvo = document.getElementById(cadeira);
        if (!alvo) return;

        const destino = alvo.getBoundingClientRect();
        const quantidade = jogador.quantidade_cartas || 3;

        for (let indice = 0; indice < quantidade; ++indice) {
            setTimeout(() => {
                const carta = document.createElement("div");
                carta.className = "carta-verso carta-animada";
                carta.style.left = `${origem.left + origem.width / 2}px`;
                carta.style.top = `${origem.top + origem.height / 2}px`;
                document.body.appendChild(carta);

                requestAnimationFrame(() => {
                    carta.style.left = `${destino.left + destino.width / 2}px`;
                    carta.style.top = `${destino.top + destino.height / 2}px`;
                    carta.style.transform = "translate(-50%, -50%) scale(.5) rotate(720deg)";
                    carta.style.opacity = "0";
                });

                setTimeout(() => carta.remove(), 650);
            }, atraso);
            atraso += 80;
        }
    });
}

function renderizarPainelQuedas() {
    let painel = document.getElementById("painel-quedas");
    if (!painel) {
        painel = document.createElement("div");
        painel.id = "painel-quedas";
        document.querySelector(".mesa-de-pano-verde").appendChild(painel);

        const estilo = document.createElement("style");
        estilo.textContent = `
            #painel-quedas {
                position: absolute;
                top: 110px;
                left: 28px;
                z-index: 30;
                display: flex;
                flex-direction: column;
                gap: 10px;
                padding: 15px 20px;
                background: rgba(2, 6, 23, 0.78);
                border: 1px solid rgba(255, 255, 255, 0.12);
                border-radius: 10px;
                box-shadow: 0 10px 30px rgba(0,0,0,0.4);
                backdrop-filter: blur(9px);
            }
            .marcador-queda {
                display: flex;
                align-items: center;
                gap: 12px;
                font-size: 0.85rem;
                color: #cbd5e1;
                font-weight: 600;
                text-transform: uppercase;
                letter-spacing: 1px;
            }
            .bolinha {
                width: 14px;
                height: 14px;
                border-radius: 50%;
                background: rgba(255,255,255,0.1);
                border: 2px solid rgba(255,255,255,0.2);
                transition: all 0.3s;
            }
            .bolinha.eq1 { background: #3b82f6; border-color: #60a5fa; box-shadow: 0 0 12px rgba(59, 130, 246, 0.6); }
            .bolinha.eq2 { background: #ef4444; border-color: #f87171; box-shadow: 0 0 12px rgba(239, 68, 68, 0.6); }
            .bolinha.empate { background: #f59e0b; border-color: #fbbf24; box-shadow: 0 0 12px rgba(245, 158, 11, 0.6); }
            @media (max-width: 1000px) { #painel-quedas { top: 90px; left: 14px; padding: 10px 15px; } .marcador-queda { font-size: 0.75rem; } }
            @media (max-width: 760px) { #painel-quedas { top: 130px; left: 10px; padding: 10px; } }
        `;
        document.head.appendChild(estilo);
    }

    const getClasseBolinha = (vencedor) => {
        if (vencedor === 1) return "eq1";
        if (vencedor === 2) return "eq2";
        if (vencedor === 0) return "empate";
        return "";
    };

    const getTexto = (vencedor) => {
        if (vencedor === undefined) return "---";
        if (vencedor === 0) return "Empate";
        return nomeEquipe(vencedor);
    };

    painel.innerHTML = `
        <div class="marcador-queda">
            <div class="bolinha ${getClasseBolinha(historicoQuedas[1])}"></div> 
            1ª Queda: <span style="color: #fff; margin-left: 4px;">${getTexto(historicoQuedas[1])}</span>
        </div>
        <div class="marcador-queda">
            <div class="bolinha ${getClasseBolinha(historicoQuedas[2])}"></div> 
            2ª Queda: <span style="color: #fff; margin-left: 4px;">${getTexto(historicoQuedas[2])}</span>
        </div>
        <div class="marcador-queda">
            <div class="bolinha ${getClasseBolinha(historicoQuedas[3])}"></div> 
            3ª Queda: <span style="color: #fff; margin-left: 4px;">${getTexto(historicoQuedas[3])}</span>
        </div>
    `;
}

function mostrarToastQueda(mensagem, vencedor) {
    let toast = document.getElementById("toast-queda");
    
    if (!toast) {
        toast = document.createElement("div");
        toast.id = "toast-queda";
        document.body.appendChild(toast);

        const estilo = document.createElement("style");
        estilo.textContent = `
            .toast-notificacao { position: absolute; top: 25px; right: 30px; z-index: 999; display: flex; align-items: center; gap: 12px; padding: 15px 25px; background: rgba(15, 23, 42, 0.95); border-left: 5px solid #f0c040; border-radius: 8px; box-shadow: 0 10px 30px rgba(0,0,0,0.6); color: #f8fafc; font-family: 'Cinzel', serif; font-weight: 600; letter-spacing: 1px; transform: translateX(150%); transition: transform 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275); }
            .toast-notificacao.mostrar { transform: translateX(0); }
        `;
        document.head.appendChild(estilo);
    }

    let cor = "#94a3b8"; 
    let icone = "⚖️";

    if (vencedor === 1) { cor = "#60a5fa"; icone = "🏆"; } 
    else if (vencedor === 2) { cor = "#f87171"; icone = "🏆"; }

    toast.className = "toast-notificacao";
    toast.style.borderLeftColor = cor;
    toast.innerHTML = `<span style="font-size: 1.5rem;">${icone}</span> <span>${escaparHtml(mensagem)}</span>`;
    
    void toast.offsetWidth; 
    toast.classList.add("mostrar");
    
    setTimeout(() => { toast.classList.remove("mostrar"); }, 3000);
}

function bloquearInteracaoTemporariamente(tempo) {
    let overlay = document.getElementById("overlay-bloqueio");
    if (overlay) overlay.remove();

    overlay = document.createElement("div");
    overlay.id = "overlay-bloqueio";
    overlay.style.position = "fixed";
    overlay.style.inset = "0";
    overlay.style.zIndex = "998"; 
    overlay.style.cursor = "wait"; 
    document.body.appendChild(overlay);

    setTimeout(() => {
        if (document.body.contains(overlay)) { overlay.remove(); }
    }, tempo);
}