let socket = null;
let estadoAtual = null;
let selecionadas = [];
let tentativaReconexao = null;

let ultimaMaoLocal = [];
let descarteLocalAtual = [];
let descarteAdversariosAtual = [];
let aguardandoAnimacaoTroca = false;

const $ = (id) => document.getElementById(id);

const sala = new URLSearchParams(location.search).get("sala") || "";

const telaConexao = $("tela-conexao");
const telaJogo = $("tela-jogo");
const nomeInput = $("nome-jogador");
const btnConectar = $("btn-conectar");
const mensagemConexao = $("mensagem-conexao");

const maoLocal = $("mao-local");
const adversarios = $("adversarios");

const btnConfirmar = $("btn-confirmar");
const btnNovaRodada = $("btn-nova-rodada");

const slots = [...document.querySelectorAll("#slots-troca span")];

const descarteLocal = $("descarte-local");
const descarteAdversarios = $("descarte-adversarios");

inicializar();

function inicializar() {
    $("sala-conexao").textContent = sala ? `Sala: ${sala}` : "Sala não informada";
    $("sala-jogo").textContent = sala || "---";

    nomeInput.value = localStorage.getItem("poker_nome") || "";

    btnConectar.onclick = conectar;
    btnConfirmar.onclick = confirmarTrocaComAnimacao;
    btnNovaRodada.onclick = novaRodada;

    nomeInput.onkeydown = (evento) => {
        if (evento.key === "Enter") {
            conectar();
        }
    };

    limparDescartes();

    if (!sala) {
        mensagemConexao.textContent = "O link não possui o código da sala.";
        btnConectar.disabled = true;
    }
}

/* =============================== */
/* CONEXÃO */
/* =============================== */

function conectar() {
    const nome = nomeInput.value.trim();

    if (!sala) {
        return;
    }

    if (nome.length < 2) {
        mensagemConexao.textContent = "Digite um nome com pelo menos 2 caracteres.";
        nomeInput.focus();
        return;
    }

    localStorage.setItem("poker_nome", nome);

    mensagemConexao.textContent = "Conectando...";
    btnConectar.disabled = true;
    btnConectar.textContent = "Conectando...";

    abrirWebSocket();
}

function abrirWebSocket() {
    if (socket && socket.readyState < 2) {
        socket.close();
    }

    const protocolo = location.protocol === "https:" ? "wss" : "ws";
    socket = new WebSocket(`${protocolo}://${location.host}/ws/poker`);

    socket.onopen = () => {
        atualizarConexao("conectando", "Conectando à sala");
    };

    socket.onmessage = (evento) => {
        let mensagem;

        try {
            mensagem = JSON.parse(evento.data);
        } catch {
            mostrarErro("Mensagem inválida recebida do servidor.");
            return;
        }

        processarMensagem(mensagem);
    };

    socket.onerror = () => {
        atualizarConexao("desconectado", "Erro de conexão");
    };

    socket.onclose = () => {
        atualizarConexao("desconectado", "Conexão perdida");

        if (!telaJogo.classList.contains("escondido")) {
            $("mensagem-principal").textContent = "Conexão perdida";
            $("mensagem-secundaria").textContent = "Tentando reconectar...";

            clearTimeout(tentativaReconexao);
            tentativaReconexao = setTimeout(abrirWebSocket, 1500);
        } else {
            btnConectar.disabled = false;
            btnConectar.textContent = "Conectar à mesa";
        }
    };
}

function processarMensagem(mensagem) {
    if (mensagem.tipo === "conectado") {
        enviarEntrada();
        return;
    }

    if (mensagem.tipo === "entrada_confirmada" || mensagem.tipo === "reconexao_confirmada") {
        telaConexao.classList.add("escondido");
        telaJogo.classList.remove("escondido");
        atualizarConexao("conectado", "Conectado à sala");
        return;
    }

    if (mensagem.tipo === "estado_jogo") {
        estadoAtual = mensagem;

        const faseAnterior = estadoAtual?.fase;
        atualizarTela(mensagem, faseAnterior);

        return;
    }

    if (mensagem.tipo === "erro") {
        mostrarErro(mensagem.mensagem || mensagem.erro || "Erro desconhecido.");
    }
}

function enviarEntrada() {
    enviar({
        tipo: "entrar_sala",
        sala: sala,
        nome: localStorage.getItem("poker_nome") || nomeInput.value.trim(),
        token: obterToken()
    });
}

function obterToken() {
    const chave = `poker_token_${sala}`;
    let token = localStorage.getItem(chave);

    if (!token) {
        token = crypto.randomUUID
            ? crypto.randomUUID()
            : `poker-${Date.now()}-${Math.random().toString(36).slice(2)}`;

        localStorage.setItem(chave, token);
    }

    return token;
}

/* =============================== */
/* ATUALIZAÇÃO PRINCIPAL */
/* =============================== */

function atualizarTela(estado) {
    const jogadores = Array.isArray(estado.jogadores) ? estado.jogadores : [];
    const jogadorLocal = jogadores.find((jogador) => jogador.id === estado.meu_id);

    const conectados = estado.jogadores_conectados ?? 0;
    const maximo = estado.max_jogadores ?? jogadores.length;

    $("rodada").textContent = estado.rodada ?? 0;
    $("empates").textContent = estado.empates ?? 0;
    $("modo").textContent = estado.modo === "COMPUTADOR" ? "Computador" : "Multiplayer";
    $("quantidade-jogadores").textContent = `${conectados}/${maximo}`;

    $("jogadores-conexao").textContent = `Jogadores: ${conectados}/${maximo}`;
    $("fase-conexao").textContent = nomeFase(estado.fase);

    $("descricao-partida").textContent =
        estado.modo === "COMPUTADOR"
            ? "Partida contra o computador."
            : `Mesa para ${maximo} jogadores.`;

    $("mensagem-principal").textContent = estado.mensagem || nomeFase(estado.fase);
    $("mensagem-secundaria").textContent = descricaoFase(estado);

    atualizarLocal(jogadorLocal, estado);
    atualizarAdversarios(jogadores.filter((jogador) => jogador.id !== estado.meu_id));

    atualizarControles(estado);
    atualizarSlots();
    atualizarStatusConfirmacao(jogadorLocal, estado);
    atualizarDescartesPeloEstado(estado, jogadores, jogadorLocal);
}

/* =============================== */
/* JOGADOR LOCAL */
/* =============================== */

function atualizarLocal(jogador, estado) {
    $("nome-local").textContent =
        jogador?.nome || localStorage.getItem("poker_nome") || "Jogador";

    $("pontos-local").textContent = jogador?.pontos ?? 0;

    $("jogada-local").textContent =
        jogador?.jogada && jogador.jogada !== "Oculta"
            ? jogador.jogada
            : "Jogada ainda não revelada";

    const mao =
        Array.isArray(estado.minha_mao) && estado.minha_mao.length
            ? estado.minha_mao
            : jogador?.mao || [];

    ultimaMaoLocal = mao;

    desenharMao(maoLocal, mao, estado.pode_confirmar_troca === true);
}

function atualizarStatusConfirmacao(jogador, estado) {
    const status = $("status-confirmacao");

    status.classList.remove("confirmado", "aguardando");

    if (estado.fase === "AGUARDANDO_OUTROS_JOGADORES") {
        status.textContent = "Troca confirmada. Aguardando os outros jogadores.";
        status.classList.add("aguardando");
        return;
    }

    if (jogador?.confirmou_troca) {
        status.textContent = "Troca confirmada";
        status.classList.add("confirmado");
        return;
    }

    if (estado.pode_confirmar_troca) {
        status.textContent = "Escolha até 3 cartas para trocar";
        return;
    }

    status.textContent = "Troca não confirmada";
}

/* =============================== */
/* ADVERSÁRIOS */
/* =============================== */

function atualizarAdversarios(lista) {
    adversarios.innerHTML = "";

    lista.forEach((jogador) => {
        const area = document.createElement("article");

        area.className = `adversario${jogador.conectado ? "" : " desconectado"}`;

        area.innerHTML = `
            <div class="adversario-cabecalho">
                <div>
                    <h3>${escapar(jogador.nome || `Jogador ${jogador.id + 1}`)}</h3>
                    <p>${estadoJogador(jogador)}</p>
                </div>

                <div class="pontos">
                    <span>Pontos</span>
                    <strong>${jogador.pontos ?? 0}</strong>
                </div>
            </div>

            <div class="mao mao-adversario"></div>

            <p>
                ${
                    jogador.jogada && jogador.jogada !== "Oculta"
                        ? `Jogada: ${escapar(jogador.jogada)}`
                        : `${jogador.quantidade_ultima_troca ?? 0} carta(s) trocada(s)`
                }
            </p>
        `;

        desenharMao(
            area.querySelector(".mao-adversario"),
            Array.isArray(jogador.mao) ? jogador.mao : [],
            false
        );

        adversarios.appendChild(area);
    });
}

/* =============================== */
/* DESENHO DAS CARTAS */
/* =============================== */

function desenharMao(container, cartas, selecionavel) {
    container.innerHTML = "";

    cartas.forEach((carta, indice) => {
        const elemento = criarElementoCarta(carta, indice, selecionavel);
        container.appendChild(elemento);
    });
}

function criarElementoCarta(carta, indice, selecionavel) {
    const elemento = document.createElement("div");

    elemento.className = "carta";
    elemento.dataset.indice = indice;

    if (!carta || carta.oculta || carta.valor == null || carta.naipe == null) {
        elemento.classList.add("oculta");
    } else {
        elemento.style.backgroundImage = `url("${caminhoCarta(carta)}")`;
        elemento.classList.add(corCarta(carta));
    }

    if (selecionavel && !elemento.classList.contains("oculta")) {
        elemento.classList.add("selecionavel");

        if (selecionadas.includes(indice)) {
            elemento.classList.add("selecionada");
        }

        elemento.onclick = () => alternarCarta(indice);
    }

    return elemento;
}

function caminhoCarta(carta) {
    const naipes = ["clubs", "hearts", "spades", "diamonds"];
    const valores = [
        "",
        "ace",
        "02",
        "03",
        "04",
        "05",
        "06",
        "07",
        "08",
        "09",
        "10",
        "jack",
        "queen",
        "king"
    ];

    return `/assets/cartas/${naipes[Number(carta.naipe)]}_${valores[Number(carta.valor)]}.png`;
}

function corCarta(carta) {
    const naipe = Number(carta.naipe);

    if (naipe === 1 || naipe === 3) {
        return "vermelha";
    }

    return "preta";
}

function textoCarta(carta) {
    if (!carta || carta.oculta) {
        return "🂠";
    }

    const valores = [
        "",
        "A",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "10",
        "J",
        "Q",
        "K"
    ];

    const simbolos = ["♣", "♥", "♠", "♦"];

    return `${valores[Number(carta.valor)] || "?"}${simbolos[Number(carta.naipe)] || "?"}`;
}

/* =============================== */
/* SELEÇÃO E TROCA */
/* =============================== */

function alternarCarta(indice) {
    if (!estadoAtual?.pode_confirmar_troca) {
        return;
    }

    const posicao = selecionadas.indexOf(indice);

    if (posicao >= 0) {
        selecionadas.splice(posicao, 1);
    } else {
        if (selecionadas.length >= 3) {
            $("mensagem-secundaria").textContent = "Você pode selecionar no máximo três cartas.";
            return;
        }

        selecionadas.push(indice);
    }

    const jogadorLocal = estadoAtual.jogadores?.find((jogador) => jogador.id === estadoAtual.meu_id);

    const mao =
        Array.isArray(estadoAtual.minha_mao) && estadoAtual.minha_mao.length
            ? estadoAtual.minha_mao
            : jogadorLocal?.mao || [];

    desenharMao(maoLocal, mao, true);
    atualizarSlots();
    atualizarControles(estadoAtual);
}

function confirmarTrocaComAnimacao() {
    if (!estadoAtual?.pode_confirmar_troca) {
        return;
    }

    const indices = [...selecionadas];

    descarteLocalAtual = indices
        .map((indice) => ultimaMaoLocal[indice])
        .filter(Boolean);

    animarCartasSelecionadas();
    desenharDescarteLocal(descarteLocalAtual);

    $("status-confirmacao").textContent =
        indices.length === 0
            ? "Mantendo todas as cartas..."
            : "Cartas jogadas na mesa...";

    $("status-confirmacao").classList.add("aguardando");

    btnConfirmar.disabled = true;

    aguardandoAnimacaoTroca = true;

    setTimeout(() => {
        enviar({
            tipo: "acao_jogo",
            acao: "CONFIRMAR_TROCA",
            indices: indices
        });

        selecionadas = [];
        atualizarSlots();
        aguardandoAnimacaoTroca = false;
    }, 520);
}

function animarCartasSelecionadas() {
    const cartas = [...maoLocal.querySelectorAll(".carta")];

    selecionadas.forEach((indice) => {
        if (cartas[indice]) {
            cartas[indice].classList.add("jogando-na-mesa");
        }
    });
}

/* =============================== */
/* DESCARTES NA MESA */
/* =============================== */

function limparDescartes() {
    descarteLocalAtual = [];
    descarteAdversariosAtual = [];

    desenharDescarteLocal([]);
    desenharDescarteAdversarios(0);
}

function atualizarDescartesPeloEstado(estado, jogadores, jogadorLocal) {
    if (estado.fase === "ESCOLHENDO_TROCAS" && !jogadorLocal?.confirmou_troca && !aguardandoAnimacaoTroca) {
        limparDescartes();
        return;
    }

    if (estado.fase === "RESULTADO" || estado.fase === "AGUARDANDO_OUTROS_JOGADORES") {
        const adversariosLista = jogadores.filter((jogador) => jogador.id !== estado.meu_id);

        const totalTrocasAdversarios = adversariosLista.reduce((total, jogador) => {
            return total + (jogador.quantidade_ultima_troca ?? 0);
        }, 0);

        desenharDescarteAdversarios(Math.min(totalTrocasAdversarios, 3));

        if (jogadorLocal?.confirmou_troca && descarteLocalAtual.length === 0) {
            const quantidade = jogadorLocal.quantidade_ultima_troca ?? 0;
            desenharDescarteLocalOculto(quantidade);
        }
    }
}

function desenharDescarteLocal(cartas) {
    if (!descarteLocal) {
        return;
    }

    prepararSlotsDescarte(descarteLocal);

    const slotsLocais = [...descarteLocal.querySelectorAll(".slot-descarte")];

    cartas.slice(0, 3).forEach((carta, indice) => {
        const slot = slotsLocais[indice];

        if (!slot) {
            return;
        }

        slot.classList.add("ocupado");
        slot.innerHTML = "";

        const elemento = document.createElement("div");
        elemento.className = `carta-descarte ${corCarta(carta)}`;
        elemento.textContent = textoCarta(carta);

        slot.appendChild(elemento);
    });
}

function desenharDescarteLocalOculto(quantidade) {
    if (!descarteLocal) {
        return;
    }

    prepararSlotsDescarte(descarteLocal);

    const slotsLocais = [...descarteLocal.querySelectorAll(".slot-descarte")];

    for (let i = 0; i < Math.min(quantidade, 3); i++) {
        const slot = slotsLocais[i];

        if (!slot) {
            continue;
        }

        slot.classList.add("ocupado");
        slot.innerHTML = `<div class="carta-descarte preta">🂠</div>`;
    }
}

function desenharDescarteAdversarios(quantidade) {
    if (!descarteAdversarios) {
        return;
    }

    prepararSlotsDescarte(descarteAdversarios);

    const slotsAdversarios = [...descarteAdversarios.querySelectorAll(".slot-descarte")];

    for (let i = 0; i < Math.min(quantidade, 3); i++) {
        const slot = slotsAdversarios[i];

        if (!slot) {
            continue;
        }

        slot.classList.add("ocupado");
        slot.innerHTML = `<div class="carta-descarte preta">🂠</div>`;
    }
}

function prepararSlotsDescarte(container) {
    container.innerHTML = "";

    for (let i = 0; i < 3; i++) {
        const slot = document.createElement("span");
        slot.className = "slot-descarte";
        container.appendChild(slot);
    }
}

/* =============================== */
/* BOTÕES E SLOTS */
/* =============================== */

function atualizarControles(estado) {
    btnConfirmar.disabled = !estado.pode_confirmar_troca;

    if (estado.pode_confirmar_troca) {
        btnConfirmar.textContent =
            selecionadas.length > 0
                ? `Jogar ${selecionadas.length} carta(s) na mesa`
                : "Manter todas as cartas";
    } else if (estado.fase === "AGUARDANDO_OUTROS_JOGADORES") {
        btnConfirmar.textContent = "Aguardando adversários";
    } else if (estado.fase === "RESULTADO") {
        btnConfirmar.textContent = "Troca finalizada";
    } else {
        btnConfirmar.textContent = "Aguardando";
    }

    btnNovaRodada.classList.toggle("escondido", !estado.pode_iniciar_nova_rodada);
    btnNovaRodada.disabled = !estado.pode_iniciar_nova_rodada;
}

function atualizarSlots() {
    slots.forEach((slot, indice) => {
        slot.classList.toggle("ativo", indice < selecionadas.length);
    });

    $("contador-selecionadas").textContent = `Selecionadas: ${selecionadas.length}/3`;
}

function novaRodada() {
    limparDescartes();
    selecionadas = [];
    atualizarSlots();

    enviar({
        tipo: "acao_jogo",
        acao: "NOVA_RODADA"
    });
}

/* =============================== */
/* ENVIO E ERROS */
/* =============================== */

function enviar(objeto) {
    if (socket?.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify(objeto));
    } else {
        mostrarErro("A conexão com o servidor não está aberta.");
    }
}

function atualizarConexao(classe, texto) {
    const ponto = $("ponto-conexao");

    ponto.className = `ponto ${classe}`;
    $("texto-conexao").textContent = texto;
}

function mostrarErro(texto) {
    if (!telaConexao.classList.contains("escondido")) {
        mensagemConexao.textContent = texto;
        btnConectar.disabled = false;
        btnConectar.textContent = "Conectar à mesa";
    } else {
        $("mensagem-principal").textContent = "Não foi possível realizar a ação";
        $("mensagem-secundaria").textContent = texto;
    }
}

/* =============================== */
/* TEXTOS AUXILIARES */
/* =============================== */

function nomeFase(fase) {
    return ({
        AGUARDANDO_JOGADORES: "Aguardando jogadores",
        ESCOLHENDO_TROCAS: "Escolhendo trocas",
        AGUARDANDO_OUTROS_JOGADORES: "Aguardando jogadores",
        RESULTADO: "Resultado da rodada"
    })[fase] || "Aguardando estado";
}

function descricaoFase(estado) {
    if (estado.fase === "AGUARDANDO_JOGADORES") {
        return "A partida começará quando todos entrarem.";
    }

    if (estado.fase === "ESCOLHENDO_TROCAS") {
        return estado.pode_confirmar_troca
            ? "Selecione até três cartas. Ao confirmar, elas serão jogadas na mesa."
            : "Aguarde sua ação ser liberada.";
    }

    if (estado.fase === "AGUARDANDO_OUTROS_JOGADORES") {
        return "Sua troca foi registrada. Aguarde os demais jogadores.";
    }

    if (estado.fase === "RESULTADO") {
        if (estado.vencedor === -1) {
            return "A rodada terminou empatada.";
        }

        return estado.vencedor === estado.meu_id
            ? "Você venceu esta rodada."
            : "Outro jogador venceu esta rodada.";
    }

    return estado.mensagem || "";
}

function estadoJogador(jogador) {
    if (jogador.computador) {
        return "Computador";
    }

    if (!jogador.conectado) {
        return "Desconectado";
    }

    return jogador.confirmou_troca ? "Troca confirmada" : "Escolhendo cartas";
}

function escapar(texto) {
    const elemento = document.createElement("div");
    elemento.textContent = texto;
    return elemento.innerHTML;
}