let socket = null;
let cartaSelecionadaIndice = null;
let estadoAtual = null;
let meuId = null;
let vitoriaExibida = false;
let temporizadorModal = null;
let temporizadorReconexao = null;
let temporizadorPing = null;
let tentativasReconexao = 0;
let encerramentoManual = false;

const minhaMao = document.getElementById("minha-mao");
const mesaDescarte = document.getElementById("mesa-descarte");
const qtdCartas = document.getElementById("qtd-cartas");
const cartaVira = document.getElementById("carta-vira");
const textoCoringa = document.getElementById("texto-coringa");
const jogadorAtualTexto = document.getElementById("jogador-atual");
const indicadorTurno = document.getElementById("indicador-turno");
const indicadorTurnoTexto = document.getElementById("indicador-turno-texto");
const indicadorTurnoFase = document.getElementById("indicador-turno-fase");
const mensagemAjuda = document.getElementById("mensagem-ajuda");
const btnComprarMonte = document.getElementById("btn-comprar-monte");
const btnComprarMesa = document.getElementById("btn-comprar-mesa");
const btnBater = document.getElementById("btn-bater");
const btnOrganizar = document.getElementById("btn-organizar");

const parametros = new URLSearchParams(window.location.search);
const idSala = parametros.get("sala") || "global";
const quantidadeJogadores = Number(parametros.get("jogadores") || 2);

iniciarWebSocket();

function iniciarWebSocket() {
    if (socket && (socket.readyState === WebSocket.OPEN || socket.readyState === WebSocket.CONNECTING)) return;

    const protocolo = window.location.protocol === "https:" ? "wss:" : "ws:";
    socket = new WebSocket(`${protocolo}//${window.location.host}/ws/pife`);

    atualizarStatusConexao("Conectando...");

    socket.onopen = () => {
        tentativasReconexao = 0;
        atualizarStatusConexao("Conectado");
        entrarNaSala();
        iniciarPing();
    };

    socket.onmessage = evento => {
        try {
            processarMensagem(JSON.parse(evento.data));
        } catch {
            console.error("Mensagem inválida:", evento.data);
        }
    };

    socket.onerror = erro => console.error("Erro no WebSocket:", erro);

    socket.onclose = () => {
        pararPing();

        if (encerramentoManual) {
            atualizarStatusConexao("Desconectado");
            return;
        }

        atualizarStatusConexao("Conexão perdida. Reconectando...");
        agendarReconexao();
    };
}

function obterTokenReconexao() {
    const chave = `tokenPife_${idSala}`;
    let token = localStorage.getItem(chave);

    if (!token) {
        token = crypto.randomUUID ? crypto.randomUUID() : `${Date.now()}-${Math.random().toString(16).slice(2)}`;
        localStorage.setItem(chave, token);
    }

    return token;
}

function entrarNaSala() {
    enviarMensagem({
        tipo: "entrar_sala",
        sala: idSala,
        jogadores: quantidadeJogadores,
        token: obterTokenReconexao()
    });
}

function enviarMensagem(mensagem) {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
        console.warn("WebSocket não está conectado.");
        return false;
    }

    socket.send(JSON.stringify(mensagem));
    return true;
}

function enviarAcao(acao, dados = {}) {
    if (!enviarMensagem({tipo: "acao_jogo", acao, ...dados})) {
        mostrarModal("Sem conexão com o servidor.");
    }
}

function agendarReconexao() {
    if (temporizadorReconexao) return;

    tentativasReconexao++;
    const atraso = Math.min(1000 * tentativasReconexao, 5000);

    temporizadorReconexao = setTimeout(() => {
        temporizadorReconexao = null;
        iniciarWebSocket();
    }, atraso);
}

function iniciarPing() {
    pararPing();

    temporizadorPing = setInterval(() => {
        enviarMensagem({tipo: "ping"});
    }, 10000);
}

function pararPing() {
    if (!temporizadorPing) return;

    clearInterval(temporizadorPing);
    temporizadorPing = null;
}

function atualizarStatusConexao(texto) {
    const elemento = document.getElementById("status-conexao");
    if (elemento) elemento.textContent = texto;
}

function processarMensagem(mensagem) {
    if (mensagem.tipo === "conectado" || mensagem.tipo === "pong") return;

    if (mensagem.tipo === "entrada_confirmada" || mensagem.tipo === "reconexao_confirmada") {
        meuId = Number(mensagem.idJogador);
        atualizarStatusConexao(mensagem.reconectado ? "Reconectado" : "Conectado");
        return;
    }

    if (mensagem.tipo === "estado_jogo") {
        meuId = Number(mensagem.meu_id);
        renderizarEstado(mensagem);
        return;
    }

    if (mensagem.tipo === "erro") {
        mostrarModal(mensagem.mensagem || mensagem.erro || "Erro no servidor.");
    }
}

function renderizarEstado(estado) {
    estadoAtual = estado;

    renderizarMao(estado.minha_mao || []);
    renderizarMesa(estado.mesa || []);
    renderizarVira(estado.vira);

    qtdCartas.textContent = (estado.minha_mao || []).length;

    if (jogadorAtualTexto) {
        jogadorAtualTexto.textContent = estado.partida_iniciada ? `Jogador ${estado.jogador_atual + 1}` : "Aguardando";
    }

    atualizarIndicadorTurno(estado);
    atualizarBotoes(estado);

    cartaSelecionadaIndice = null;

    if (estado.jogo_finalizado) mostrarResultadoFinal(estado);
}

function renderizarMao(cartas) {
    minhaMao.innerHTML = "";

    cartas.forEach((carta, indice) => {
        const elemento = criarCarta(carta);

        elemento.onclick = () => {
            if (!estadoAtual?.pode_descartar) return;

            document.querySelectorAll("#minha-mao .carta").forEach(carta => carta.classList.remove("selecionada"));

            elemento.classList.add("selecionada");
            cartaSelecionadaIndice = indice;
        };

        minhaMao.appendChild(elemento);
    });
}

function renderizarMesa(cartas) {
    mesaDescarte.innerHTML = "";

    if (!cartas.length) {
        mesaDescarte.innerHTML = '<div class="carta carta-vazia">Descarte</div>';
        return;
    }

    mesaDescarte.appendChild(criarCarta(cartas.at(-1)));
}

function renderizarVira(vira) {
    if (!cartaVira || !textoCoringa) return;

    cartaVira.innerHTML = "";

    if (!vira) {
        cartaVira.innerHTML = '<div class="carta carta-vazia">Vira</div>';
        textoCoringa.textContent = "aguardando";
        return;
    }

    cartaVira.appendChild(criarCarta(vira));

    const valorCoringa = Number(vira.valor) === 13 ? 1 : Number(vira.valor) + 1;

    textoCoringa.textContent = cartaParaTexto({
        valor: valorCoringa,
        naipe: vira.naipe
    });
}

function criarCarta(carta) {
    const imagem = document.createElement("img");

    imagem.className = "carta";
    imagem.src = caminhoImagemCarta(carta);
    imagem.alt = cartaParaTexto(carta);
    imagem.draggable = false;

    return imagem;
}

function caminhoImagemCarta(carta) {
    const naipes = {
        0: "clubs",
        1: "hearts",
        2: "spades",
        3: "diamonds"
    };

    const valores = {
        1: "ace",
        2: "02",
        3: "03",
        4: "04",
        5: "05",
        6: "06",
        7: "07",
        8: "08",
        9: "09",
        10: "10",
        11: "jack",
        12: "queen",
        13: "king"
    };

    const nomeNaipe = naipes[Number(carta.naipe)];
    const nomeValor = valores[Number(carta.valor)];

    if (!nomeNaipe || !nomeValor) {
        console.error("Carta inválida:", carta);
        return "";
    }

    return `/assets/cartas/${nomeNaipe}_${nomeValor}.png`;
}

function cartaParaTexto(carta) {
    const valores = {1: "A", 11: "J", 12: "Q", 13: "K"};
    const naipes = {0: "♣", 1: "♥", 2: "♠", 3: "♦"};

    return `${valores[carta.valor] || carta.valor}${naipes[carta.naipe] || ""}`;
}

function atualizarIndicadorTurno(estado) {
    if (!indicadorTurno || !indicadorTurnoTexto || !indicadorTurnoFase || !mensagemAjuda) return;

    indicadorTurno.classList.remove("aguardando", "minha-vez", "vez-oponente", "finalizado");

    if (estado.jogo_finalizado) {
        indicadorTurno.classList.add("finalizado");
        indicadorTurnoTexto.textContent = "Partida finalizada";
        indicadorTurnoFase.textContent = "Confira o resultado";
        mensagemAjuda.textContent = "A partida terminou.";
        return;
    }

    if (!estado.partida_iniciada) {
        indicadorTurno.classList.add("aguardando");
        indicadorTurnoTexto.textContent = "Aguardando jogadores";
        indicadorTurnoFase.textContent = `${estado.jogadores_conectados}/${estado.max_jogadores} conectados`;
        mensagemAjuda.textContent = "A partida começará quando todos entrarem.";
        return;
    }

    const minhaVez = estado.jogador_atual === meuId;

    indicadorTurno.classList.add(minhaVez ? "minha-vez" : "vez-oponente");
    indicadorTurnoTexto.textContent = minhaVez ? "Sua vez" : `Vez do Jogador ${estado.jogador_atual + 1}`;

    if (estado.fase === "AGUARDANDO_COMPRA") {
        indicadorTurnoFase.textContent = minhaVez ? "Compre uma carta" : "Aguardando compra";
        mensagemAjuda.textContent = minhaVez ? "Compre uma carta do monte ou da mesa." : "Aguarde o outro jogador comprar.";
        return;
    }

    if (estado.fase === "AGUARDANDO_DESCARTE") {
        indicadorTurnoFase.textContent = minhaVez ? "Descarte uma carta" : "Aguardando descarte";
        mensagemAjuda.textContent = minhaVez ? "Selecione uma carta e clique na área de descarte." : "Aguarde o outro jogador descartar.";
        return;
    }

    indicadorTurnoFase.textContent = "Aguardando atualização";
    mensagemAjuda.textContent = "Aguarde.";
}

function atualizarBotoes(estado) {
    const finalizado = Boolean(estado.jogo_finalizado);

    btnComprarMonte.disabled = finalizado || !estado.pode_comprar_baralho;
    btnComprarMesa.disabled = finalizado || !estado.pode_comprar_mesa;
    btnBater.disabled = finalizado || !estado.pode_bater;
    btnOrganizar.disabled = finalizado || !estado.partida_iniciada;
}

function mostrarResultadoFinal(estado) {
    if (vitoriaExibida) return;

    vitoriaExibida = true;
    const venceu = estado.vencedor === meuId;

    mostrarModal(
        venceu ? "Sua mão formou uma combinação válida de Pife." : `O Jogador ${estado.vencedor + 1} venceu a partida.`,
        {
            titulo: venceu ? "Você venceu!" : "Fim de jogo",
            persistente: true,
            vitoria: venceu,
            mostrarBotao: true
        }
    );
}

function mostrarModal(texto, opcoes = {}) {
    const modal = document.getElementById("modal-notificacao");
    const titulo = document.getElementById("modal-titulo");
    const conteudo = document.getElementById("modal-texto");
    const botao = document.getElementById("modal-botao");

    if (!modal || !titulo || !conteudo || !botao) {
        console.warn(texto);
        return;
    }

    clearTimeout(temporizadorModal);

    titulo.textContent = opcoes.titulo || "Aviso";
    conteudo.textContent = texto;
    modal.classList.toggle("modal-vitoria", Boolean(opcoes.vitoria));
    botao.classList.toggle("modal-botao-oculto", !opcoes.mostrarBotao);
    modal.classList.remove("modal-oculto");

    if (!opcoes.persistente) {
        temporizadorModal = setTimeout(() => {
            modal.classList.add("modal-oculto");
            modal.classList.remove("modal-vitoria");
        }, 1500);
    }
}

btnComprarMonte.onclick = () => enviarAcao("COMPRAR_BARALHO");
btnComprarMesa.onclick = () => enviarAcao("COMPRAR_MESA");
btnBater.onclick = () => enviarAcao("BATER");
btnOrganizar.onclick = () => enviarAcao("ORGANIZAR");

mesaDescarte.onclick = () => {
    if (!estadoAtual) {
        mostrarModal("O jogo ainda não foi carregado.");
        return;
    }

    if (estadoAtual.jogo_finalizado) {
        mostrarModal("A partida já terminou.");
        return;
    }

    if (!estadoAtual.pode_descartar) {
        mostrarModal("Você não pode descartar agora.");
        return;
    }

    if (cartaSelecionadaIndice === null) {
        mostrarModal("Selecione uma carta da sua mão.");
        return;
    }

    enviarAcao("DESCARTAR", {indice: cartaSelecionadaIndice});
};

const botaoModal = document.getElementById("modal-botao");

if (botaoModal) {
    botaoModal.onclick = () => {
        window.location.href = "/pages/menu.html";
    };
}

window.addEventListener("beforeunload", () => {
    encerramentoManual = true;
    pararPing();

    if (temporizadorReconexao) {
        clearTimeout(temporizadorReconexao);
        temporizadorReconexao = null;
    }

    if (socket) socket.close();
});