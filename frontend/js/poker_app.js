let socket = null;

function conectar() {
    const protocolo = window.location.protocol === "https:" ? "wss" : "ws";
    socket = new WebSocket(`${protocolo}://${window.location.host}/ws/poker`);

    socket.onopen = () => {
        document.getElementById("status").textContent = "Conectado ao servidor do Poker.";
        enviarAcao("OBTER_ESTADO_ATUAL");
    };

    socket.onmessage = (event) => {
        const estado = JSON.parse(event.data);
        atualizarTela(estado);
    };

    socket.onclose = () => {
        document.getElementById("status").textContent = "Conexão encerrada com o servidor.";
    };

    socket.onerror = () => {
        document.getElementById("status").textContent = "Erro na conexão com o servidor.";
    };
}

function enviarAcao(acao) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({ acao: acao }));
    }
}

function atualizarTela(estado) {
    desenharMao("mao-jogador1", estado.jogador1.mao);
    desenharMao("mao-jogador2", estado.jogador2.mao);

    document.getElementById("jogada-jogador1").textContent = estado.jogador1.jogada;
    document.getElementById("jogada-jogador2").textContent = estado.jogador2.jogada;

    document.getElementById("vencedor").textContent = `Vencedor: ${estado.vencedor}`;
}

function desenharMao(idElemento, mao) {
    const container = document.getElementById(idElemento);
    container.innerHTML = "";

    mao.forEach((carta) => {
        const div = document.createElement("div");
        div.classList.add("carta");

        if (carta.texto.includes("Copa") || carta.texto.includes("Ouro")) {
            div.classList.add("vermelha");
        } else {
            div.classList.add("preta");
        }

        div.textContent = carta.texto;
        container.appendChild(div);
    });
}

document.getElementById("btn-nova-rodada").addEventListener("click", () => {
    enviarAcao("NOVA_RODADA");
});

conectar();