// ==========================================
// SISTEMA DE ÁUDIO
// ==========================================
const sons = {
    jogar: new Audio("/assets/sons/jogar_carta.ogg"),
    shuffle: new Audio("/assets/sons/shuffle.mp3"),
    vitoria: new Audio("/assets/sons/victory_6.mp3"),
    click: new Audio("/assets/sons/click.mp3"),
    selecionar: new Audio("/assets/sons/selecionar.mp3") 
};

function tocarSom(nome) {
    if (sons[nome]) {
        sons[nome].currentTime = 0;
        sons[nome].play().catch(e => console.log("Erro de áudio:", e));
    }
}

// ==========================================
// VARIÁVEIS DE ESTADO
// ==========================================
let socket = null, meuId = null, cartaArrastada = null;
let ordemLocalMao = []; 
let cartaSelecionadaIndice = null;
let estadoAtualGlobal = null; 

const minhaMao = document.getElementById("minha-mao");
const monteCompra = document.getElementById("monte-compra");
const mesaDescarte = document.getElementById("mesa-descarte");
const cartaVira = document.getElementById("carta-vira");
const textoCoringa = document.getElementById("carta-coringa");
const qtdCartas = document.getElementById("qtd-cartas");
const jogadorAtualTexto = document.getElementById("jogador-atual");

const btnBater = document.getElementById("btn-bater");
const btnOrganizar = document.getElementById("btn-organizar");

iniciarWebSocket();

function iniciarWebSocket() {
    const parametros = new URLSearchParams(window.location.search);
    const sala = parametros.get("sala") || "global";
    const jogadores = Number(parametros.get("jogadores") || 2);
    const protocolo = location.protocol === "https:" ? "wss" : "ws";

    socket = new WebSocket(`${protocolo}://${location.host}/ws/pife`);

    socket.onopen = () => {
        socket.send(JSON.stringify({ tipo: "entrar_sala", sala, jogadores }));
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
        if (!msg.minha_mao) return;
        if (Number.isInteger(msg.meu_id)) meuId = msg.meu_id;

        renderizarEstado(msg);
    };

    socket.onclose = () => mostrarModal("Conexão encerrada");
}

function enviarAcao(acao, dados = {}) {
    if (!socket || socket.readyState !== WebSocket.OPEN) return;
    socket.send(JSON.stringify({ tipo: "acao_jogo", acao, ...dados }));
}

function renderizarEstado(estado) {
    estadoAtualGlobal = estado;

    renderizarMao(estado.minha_mao);
    renderizarOponentes(estado);
    renderizarMesa(estado.mesa || []);
    renderizarVira(estado.vira);

    qtdCartas.textContent = estado.minha_mao.length;
    jogadorAtualTexto.textContent = `Jogador ${estado.jogador_atual + 1}`;

    atualizarBotoesEAreas(estado);
    verificarVitoria(estado);

    cartaSelecionadaIndice = null;
}

// ==========================================
// RENDERIZAÇÃO E LÓGICA DE JOGADORES (INCLUI OPONENTES DINÂMICOS)
// ==========================================
function renderizarOponentes(estado) {
    document.getElementById('cadeira-direita').innerHTML = '';
    document.getElementById('cadeira-topo').innerHTML = '';
    document.getElementById('cadeira-esquerda').innerHTML = '';

    let listaJogadores = estado.jogadores;
    if (!listaJogadores) {
        const urlParams = new URLSearchParams(window.location.search);
        const totalParams = Number(urlParams.get("jogadores") || 2);
        const total = estado.total_jogadores || totalParams;
        listaJogadores = [];
        for(let i = 0; i < total; i++) {
            listaJogadores.push({
                id: i,
                nome: `Jogador ${i + 1}`,
                qtd_cartas: (estado.jogador_atual === i && estado.fase_turno === 'AguardandoDescarte') ? 10 : 9
            });
        }
    }

    const totalJogadores = listaJogadores.length;

    listaJogadores.forEach(j => {
        if (j.id !== meuId) {
            let posicaoRelativa = (j.id - meuId + totalJogadores) % totalJogadores;
            let idCadeira = "";

            if (totalJogadores === 4) {
                if (posicaoRelativa === 1) idCadeira = 'cadeira-direita';
                else if (posicaoRelativa === 2) idCadeira = 'cadeira-topo';
                else if (posicaoRelativa === 3) idCadeira = 'cadeira-esquerda';
            } else if (totalJogadores === 3) {
                if (posicaoRelativa === 1) idCadeira = 'cadeira-direita';
                else if (posicaoRelativa === 2) idCadeira = 'cadeira-esquerda';
            } else {
                idCadeira = 'cadeira-topo';
            }

            if (idCadeira) {
                const cadeiraDiv = document.getElementById(idCadeira);
                let htmlCartas = '<div class="mao-oponente">';
                let qtdCartasNaMao = j.qtd_cartas || j.cartas_na_mao || 9; 

                const isTurno = (estado.jogador_atual === j.id);
                const tituloColor = isTurno ? '#3fff85' : '#ffffff';

                for (let c = 0; c < qtdCartasNaMao; c++) {
                    htmlCartas += `<div class="carta-verso"></div>`;
                }
                htmlCartas += '</div>';

                cadeiraDiv.innerHTML = `
                    <h2 style="color: ${tituloColor};">${j.nome || 'Jogador ' + (j.id + 1)}</h2>
                    ${htmlCartas}
                `;
            }
        }
    });
}

function renderizarMao(cartas) {
    minhaMao.innerHTML = "";
    let lista = cartas.map((c, i) => ({ ...c, idx: i, id: `${c.valor}-${c.naipe}` }));

    lista.sort((a, b) => {
        let posA = ordemLocalMao.indexOf(a.id);
        let posB = ordemLocalMao.indexOf(b.id);
        return (posA === -1 ? 999 : posA) - (posB === -1 ? 999 : posB);
    });

    lista.forEach(item => {
        const el = criarCarta(item);
        el.dataset.idx = item.idx;
        el.dataset.id = item.id;
        el.draggable = true;

        el.ondragstart = (e) => {
            cartaArrastada = el;
            cartaSelecionadaIndice = item.idx; 
            el.classList.add('dragging');
            tocarSom('selecionar');
        };

        el.ondragend = () => {
            el.classList.remove('dragging');
            ordemLocalMao = [...minhaMao.querySelectorAll('.carta')].map(c => c.dataset.id);
        };

        el.onclick = () => {
            document.querySelectorAll('.carta').forEach(c => c.classList.remove('selecionada'));
            el.classList.add('selecionada');
            cartaSelecionadaIndice = item.idx;
            tocarSom('selecionar');
        };

        minhaMao.appendChild(el);
    });
}

minhaMao.ondragover = e => {
    e.preventDefault();
    const after = [...minhaMao.querySelectorAll('.carta:not(.dragging)')].find(c => {
        return e.clientX <= c.getBoundingClientRect().left + c.offsetWidth / 2;
    });
    after ? minhaMao.insertBefore(cartaArrastada, after) : minhaMao.appendChild(cartaArrastada);
};

// ==========================================
// RENDERIZAÇÃO DA MESA E BARALHO
// ==========================================
function renderizarMesa(cartas) {
    mesaDescarte.innerHTML = "";
    if (cartas.length === 0) {
        mesaDescarte.innerHTML = '<div class="carta carta-vazia">Descarte</div>';
        return;
    }
    mesaDescarte.appendChild(criarCarta(cartas[cartas.length - 1]));
}

function renderizarVira(vira) {
    cartaVira.innerHTML = "";
    if (!vira) return;
    cartaVira.appendChild(criarCarta(vira));
    const valorCoringa = Number(vira.valor) === 13 ? 1 : Number(vira.valor) + 1;
    textoCoringa.textContent = cartaParaTexto({ valor: valorCoringa, naipe: vira.naipe });
}

function criarCarta(carta) {
    const img = document.createElement("img");
    img.className = "carta";
    img.src = caminhoImagemCarta(carta);
    img.alt = cartaParaTexto(carta);
    img.draggable = false;
    return img;
}

function caminhoImagemCarta(carta) {
    const naipes = { 0: "clubs", 1: "hearts", 2: "spades", 3: "diamonds" };
    const valores = { 1: "ace", 2: "02", 3: "03", 4: "04", 5: "05", 6: "06", 7: "07", 8: "08", 9: "09", 10: "10", 11: "jack", 12: "queen", 13: "king" };
    return `/assets/cartas/${naipes[Number(carta.naipe)]}_${valores[Number(carta.valor)]}.png`;
}

function cartaParaTexto(carta) {
    const valores = { 1: "A", 11: "J", 12: "Q", 13: "K" };
    const naipes = { 0: "♣", 1: "♥", 2: "♠", 3: "♦" };
    return `${valores[carta.valor] || carta.valor}${naipes[carta.naipe]}`;
}

// ==========================================
// INTERAÇÕES VISUAIS E AÇÕES DE JOGO
// ==========================================
function atualizarBotoesEAreas(estado) {
    btnBater.disabled = !estado.pode_bater;
    btnOrganizar.disabled = estado.jogo_finalizado;

    const statusBox = document.querySelector('.status-box');
    if (estado.jogador_atual === meuId && !estado.jogo_finalizado) {
        statusBox.classList.add('sua-vez');
    } else {
        statusBox.classList.remove('sua-vez');
    }

    if (estado.pode_comprar_baralho) monteCompra.classList.add("ativo");
    else monteCompra.classList.remove("ativo");

    if (estado.pode_comprar_mesa) mesaDescarte.classList.add("ativo");
    else mesaDescarte.classList.remove("ativo");
}

monteCompra.onclick = () => {
    if (estadoAtualGlobal && estadoAtualGlobal.pode_comprar_baralho) {
        tocarSom('selecionar');
        enviarAcao("COMPRAR_BARALHO");
    }
};

mesaDescarte.onclick = () => {
    if (estadoAtualGlobal && estadoAtualGlobal.pode_comprar_mesa) {
        tocarSom('selecionar');
        enviarAcao("COMPRAR_MESA");
    } 
    else if (cartaSelecionadaIndice !== null) {
        tocarSom('jogar');
        enviarAcao("DESCARTAR", { indice: cartaSelecionadaIndice });
        cartaSelecionadaIndice = null;
    }
};

btnBater.onclick = () => enviarAcao("BATER");

btnOrganizar.onclick = () => {
    tocarSom('click');
    ordemLocalMao = []; 
    enviarAcao("ORGANIZAR");
};

// ==========================================
// DROPZONE (MESA DE DESCARTE)
// ==========================================
mesaDescarte.addEventListener('dragover', (e) => {
    e.preventDefault(); 
    if (cartaSelecionadaIndice !== null) {
        mesaDescarte.classList.add('drag-over');
    }
});

mesaDescarte.addEventListener('dragleave', () => {
    mesaDescarte.classList.remove('drag-over');
});

mesaDescarte.addEventListener('drop', (e) => {
    e.preventDefault();
    mesaDescarte.classList.remove('drag-over');

    if (cartaSelecionadaIndice !== null) {
        tocarSom('jogar');
        enviarAcao("DESCARTAR", { indice: cartaSelecionadaIndice });
        cartaSelecionadaIndice = null;
    }
});

// ==========================================
// NOTIFICAÇÕES E VITÓRIA
// ==========================================
let jogoJaAcabou = false;

function verificarVitoria(estado) {
    if (estado.jogo_finalizado && !jogoJaAcabou) {
        jogoJaAcabou = true;
        if (estado.vencedor === meuId) {
            mostrarModal("Você Bateu! 🎉");
            tocarSom('vitoria');
        } else {
            mostrarModal(`Jogador ${estado.vencedor + 1} Bateu!`);
        }
    } else if (!estado.jogo_finalizado) {
        jogoJaAcabou = false;
    }
}

function mostrarModal(texto) {
    const modal = document.getElementById("modal-notificacao");
    const modalTexto = document.getElementById("modal-texto");
    modalTexto.textContent = texto;
    modal.classList.remove("modal-oculto");
    setTimeout(() => modal.classList.add("modal-oculto"), 2000);
}