let socket = null;
let cartaSelecionadaIndice = null;
let estadoAtual = null;

const minhaMao = document.getElementById("minha-mao");
const mesaDescarte = document.getElementById("mesa-descarte");
const qtdCartas = document.getElementById("qtd-cartas");

const btnComprarMonte = document.getElementById("btn-comprar-monte");
const btnComprarMesa = document.getElementById("btn-comprar-mesa");
const btnBater = document.getElementById("btn-bater");
const btnOrganizar = document.getElementById("btn-organizar");

iniciarWebSocket();

function iniciarWebSocket() {
    socket = new WebSocket("ws://localhost:8080/ws/pife");

    socket.onopen = () => {
        console.log("Conectado ao servidor do Pife.");

        socket.send(JSON.stringify({
            acao: "ENTRAR_SALA",
            sala_id: "global"
        }));
    };

    socket.onmessage = (event) => {
        const msg = JSON.parse(event.data);

        console.log("Mensagem recebida:", msg);

        if (msg.erro) {
            mostrarModal(msg.erro);
            return;
        }

        estadoAtual = msg;
        renderizarEstado(msg);
    };

    socket.onclose = () => {
        console.log("Conexão com o servidor do Pife encerrada.");
        mostrarModal("Conexão encerrada.");
    };

    socket.onerror = () => {
        mostrarModal("Erro na conexão com o servidor.");
    };
}

function enviarAcao(acao, dados = {}) {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
        mostrarModal("Servidor não conectado.");
        return;
    }

    socket.send(JSON.stringify({
        acao,
        ...dados
    }));
}

function renderizarEstado(estado) {
    if (!estado.minha_mao) {
        console.warn("Estado sem minha_mao:", estado);
        return;
    }

    renderizarMao(estado.minha_mao);
    renderizarMesa(estado.mesa || []);

    if (qtdCartas) {
        qtdCartas.textContent = estado.minha_mao.length;
    }

    atualizarInfoTurno(estado);

    cartaSelecionadaIndice = null;
}

function renderizarMao(cartas) {
    minhaMao.innerHTML = "";

    cartas.forEach((carta, indice) => {
        const divCarta = criarCartaHTML(carta);

        divCarta.addEventListener("click", () => {
            selecionarCarta(divCarta, indice);
        });

        minhaMao.appendChild(divCarta);
    });
}

function renderizarMesa(cartas) {
    mesaDescarte.innerHTML = "";

    if (!cartas || cartas.length === 0) {
        const vazia = document.createElement("div");
        vazia.classList.add("carta", "carta-vazia");
        vazia.textContent = "Mesa";
        mesaDescarte.appendChild(vazia);
        return;
    }

    const ultimaCarta = cartas[cartas.length - 1];
    const divCarta = criarCartaHTML(ultimaCarta);

    mesaDescarte.appendChild(divCarta);
}

function criarCartaHTML(carta) {
    const div = document.createElement("div");
    div.classList.add("carta");

    div.textContent = cartaParaTexto(carta);

    return div;
}

function cartaParaTexto(carta) {
    const valores = {
        1: "A",
        2: "2",
        3: "3",
        4: "4",
        5: "5",
        6: "6",
        7: "7",
        8: "8",
        9: "9",
        10: "10",
        11: "J",
        12: "Q",
        13: "K"
    };

    const naipes = {
        0: "♦",
        1: "♥",
        2: "♠",
        3: "♣"
    };

    const valor = valores[carta.valor] ?? "?";
    const naipe = naipes[carta.naipe] ?? "?";

    return `${valor}${naipe}`;
}

function selecionarCarta(elemento, indice) {
    document.querySelectorAll("#minha-mao .carta").forEach((carta) => {
        carta.classList.remove("selecionada");
    });

    cartaSelecionadaIndice = indice;
    elemento.classList.add("selecionada");
}

function atualizarInfoTurno(estado) {
    const infoTurno = document.getElementById("info-turno");

    if (!infoTurno) {
        return;
    }

    if (estado.jogador_local === estado.jogador_atual) {
        infoTurno.textContent = "Sua vez";
    } else {
        infoTurno.textContent = "Vez do outro jogador";
    }
}

if (btnComprarMonte) {
    btnComprarMonte.addEventListener("click", () => {
        enviarAcao("COMPRAR_BARALHO");
    });
}

if (btnComprarMesa) {
    btnComprarMesa.addEventListener("click", () => {
        enviarAcao("COMPRAR_MESA");
    });
}

if (btnBater) {
    btnBater.addEventListener("click", () => {
        enviarAcao("BATER");
    });
}

if (btnOrganizar) {
    btnOrganizar.addEventListener("click", () => {
        enviarAcao("ORGANIZAR");
    });
}

if (mesaDescarte) {
    mesaDescarte.addEventListener("click", () => {
        if (cartaSelecionadaIndice === null) {
            mostrarModal("Selecione uma carta antes de descartar.");
            return;
        }

        enviarAcao("DESCARTAR", {
            indice: cartaSelecionadaIndice
        });
    });
}

function mostrarModal(texto) {
    const modal = document.getElementById("modal-notificacao");
    const modalTexto = document.getElementById("modal-texto");

    if (!modal || !modalTexto) {
        alert(texto);
        return;
    }

    modalTexto.textContent = texto;
    modal.classList.remove("modal-oculto");

    setTimeout(() => {
        modal.classList.add("modal-oculto");
    }, 1200);
}