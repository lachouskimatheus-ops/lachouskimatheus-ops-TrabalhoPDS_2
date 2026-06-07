let socket = null;
let meuId = null;
let cartaSelecionadaIndice = null;

const CAMINHO_CARTAS = "/assets/cartas/";

const minhaMao = document.getElementById("minha-mao");
const mesaDescarte = document.getElementById("mesa-descarte");
const cartaVira = document.getElementById("carta-vira");
const textoCoringa = document.getElementById("carta-coringa");
const qtdCartas = document.getElementById("qtd-cartas");
const jogadorAtualTexto = document.getElementById("jogador-atual");

const btnComprarMonte = document.getElementById("btn-comprar-monte");
const btnComprarMesa = document.getElementById("btn-comprar-mesa");
const btnBater = document.getElementById("btn-bater");
const btnOrganizar = document.getElementById("btn-organizar");

iniciarWebSocket();

function iniciarWebSocket() {
    const parametros = new URLSearchParams(window.location.search);
    const sala = parametros.get("sala") || "global";
    const jogadores = Number(parametros.get("jogadores") || 2);

    const protocolo = location.protocol === "https:" ? "wss" : "ws";

    socket = new WebSocket(
        `${protocolo}://${location.host}/ws/pife`
    );

    socket.onopen = () => {
        socket.send(JSON.stringify({
            tipo: "entrar_sala",
            sala,
            jogadores
        }));
    };

    socket.onmessage = (event) => {
        const msg = JSON.parse(event.data);

        if (msg.erro) {
            mostrarModal(msg.erro);
            return;
        }

        if (msg.tipo === "entrada_confirmada") {
            meuId = msg.idJogador;
            mostrarModal(`Jogador ${meuId + 1} conectado`);
            return;
        }

        if (!msg.minha_mao) {
            return;
        }

        if (Number.isInteger(msg.meu_id)) {
            meuId = msg.meu_id;
        }

        renderizarEstado(msg);
    };

    socket.onclose = () => {
        mostrarModal("Conexão encerrada");
    };
}

function enviarAcao(acao, dados = {}) {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
        return;
    }

    socket.send(JSON.stringify({
        tipo: "acao_jogo",
        acao,
        ...dados
    }));
}

function renderizarEstado(estado) {
    renderizarMao(estado.minha_mao);
    renderizarMesa(estado.mesa || []);
    renderizarVira(estado.vira);

    qtdCartas.textContent = estado.minha_mao.length;

    jogadorAtualTexto.textContent =
        `Jogador ${estado.jogador_atual + 1}`;

    atualizarBotoes(estado);

    cartaSelecionadaIndice = null;
}

function renderizarMao(cartas) {
    minhaMao.innerHTML = "";

    cartas.forEach((carta, indice) => {
        const elemento = criarCarta(carta);

        elemento.onclick = () => {
            document
                .querySelectorAll("#minha-mao .carta")
                .forEach(c => c.classList.remove("selecionada"));

            elemento.classList.add("selecionada");
            cartaSelecionadaIndice = indice;
        };

        minhaMao.appendChild(elemento);
    });
}

function renderizarMesa(cartas) {
    mesaDescarte.innerHTML = "";

    if (cartas.length === 0) {
        mesaDescarte.innerHTML =
            '<div class="carta carta-vazia">Descarte</div>';

        return;
    }

    mesaDescarte.appendChild(
        criarCarta(cartas[cartas.length - 1])
    );
}

function renderizarVira(vira) {
    cartaVira.innerHTML = "";

    if (!vira) {
        return;
    }

    cartaVira.appendChild(criarCarta(vira));

    const valorCoringa =
        Number(vira.valor) === 13
            ? 1
            : Number(vira.valor) + 1;

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
        console.error("Carta inválida para imagem:", carta);
        return "";
    }

    return `/assets/cartas/${nomeNaipe}_${nomeValor}.png`;
}

function cartaParaTexto(carta) {
    const valores = {
        1: "A",
        11: "J",
        12: "Q",
        13: "K"
    };

    const naipes = {
        0: "♣",
        1: "♥",
        2: "♠",
        3: "♦"
    };

    return `${valores[carta.valor] || carta.valor}${naipes[carta.naipe]}`;
}

function atualizarBotoes(estado) {
    btnComprarMonte.disabled =
        !estado.pode_comprar_baralho;

    btnComprarMesa.disabled =
        !estado.pode_comprar_mesa;

    btnBater.disabled =
        !estado.pode_bater;

    btnOrganizar.disabled =
        estado.jogo_finalizado;
}

btnComprarMonte.onclick = () =>
    enviarAcao("COMPRAR_BARALHO");

btnComprarMesa.onclick = () =>
    enviarAcao("COMPRAR_MESA");

btnBater.onclick = () =>
    enviarAcao("BATER");

btnOrganizar.onclick = () =>
    enviarAcao("ORGANIZAR");

mesaDescarte.onclick = () => {
    if (cartaSelecionadaIndice === null) {
        mostrarModal("Selecione uma carta");
        return;
    }

    enviarAcao("DESCARTAR", {
        indice: cartaSelecionadaIndice
    });
};

function mostrarModal(texto) {
    const modal = document.getElementById("modal-notificacao");
    const modalTexto = document.getElementById("modal-texto");

    modalTexto.textContent = texto;
    modal.classList.remove("modal-oculto");

    setTimeout(() => {
        modal.classList.add("modal-oculto");
    }, 1200);
}