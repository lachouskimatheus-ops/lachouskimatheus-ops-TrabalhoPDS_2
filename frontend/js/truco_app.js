// ==========================================
// SISTEMA DE ÁUDIO
// ==========================================
const CAMINHO_SONS = "/assets/sons/";
const sonsPreCarregados = {};

function preCarregarSom(nomeArquivo) {
    const audio = new Audio(`${CAMINHO_SONS}${nomeArquivo}`);
    audio.preload = "auto";
    audio.load();
    sonsPreCarregados[nomeArquivo] = audio;
}

function tocarSom(nomeArquivo) {
    const audioOriginal = sonsPreCarregados[nomeArquivo];
    if (!audioOriginal) return;
    const audio = audioOriginal.cloneNode(true);
    audio.currentTime = 0;
    audio.play().catch(() => {});
}

["jogar_carta.ogg", "shuffle.mp3", "victory_6.mp3", "click.mp3"].forEach(preCarregarSom);

// ==========================================
// CONFIGURAÇÃO INICIAL
// ==========================================
const meuNome = localStorage.getItem('jogador_nickname') || 'Você';
const urlParams = new URLSearchParams(window.location.search);
const meuId = parseInt(urlParams.get('id')) || 0;

document.addEventListener("DOMContentLoaded", () => {
    const tituloJogador = document.getElementById('nome-local');
    if (tituloJogador) tituloJogador.innerText = meuNome;
});

const socket = new WebSocket(`ws://${window.location.host}/ws/truco`);

socket.onopen = function () {
    console.log("WebSocket Truco conectado");
    socket.send(JSON.stringify({ acao: "ENTRAR", jogador_id: meuId, nome: meuNome }));
};

socket.onmessage = function (event) {
    const estado = JSON.parse(event.data);
    atualizarInterface(estado);
};

socket.onerror = function (e) {
    console.error("Erro WebSocket:", e);
};

// ==========================================
// ATUALIZAÇÃO DA INTERFACE
// ==========================================
function atualizarInterface(dados) {
    const estadoAnterior = window.estadoTrucoAtual;

    // Detecta nova distribuição de cartas para animação
    const eu = dados.jogadores ? dados.jogadores.find(j => j.id === meuId) : null;
    const euAnterior = estadoAnterior && estadoAnterior.jogadores
        ? estadoAnterior.jogadores.find(j => j.id === meuId)
        : null;

    const novaDistribuicao = (!euAnterior && eu && eu.mao.length > 0)
        || (euAnterior && eu && eu.mao.length > euAnterior.mao.length);

    window.estadoTrucoAtual = dados;

    // --- Placar ---
    document.getElementById('pontos-eq1').innerText = dados.pontos_equipe1 ?? 0;
    document.getElementById('pontos-eq2').innerText = dados.pontos_equipe2 ?? 0;

    // --- Valor da mão e queda ---
    document.getElementById('valor-mao').innerText = dados.valor_mao ?? 1;
    const quedaTextos = ["", "1ª Queda", "2ª Queda", "3ª Queda"];
    document.getElementById('queda-atual').innerText = quedaTextos[dados.queda_atual] || "";

    // --- Vira ---
    const viraDiv = document.getElementById('carta-vira');
    if (dados.vira && dados.vira.valor !== undefined) {
        const textoValor = traduzirValor(dados.vira.valor);
        const simbolo = obterSimbolo(dados.vira.naipe);
        viraDiv.innerHTML = `<span>${textoValor}</span>${gerarFigura(textoValor, dados.vira.naipe)}`;
        viraDiv.className = `carta ${dados.vira.naipe}`;
        viraDiv.setAttribute('data-naipe-simbolo', simbolo);
    } else {
        viraDiv.innerHTML = "?";
        viraDiv.className = "carta";
    }

    // --- Cartas na mesa ---
    const mesaDiv = document.getElementById('cartas-na-mesa');
    mesaDiv.innerHTML = '';
    if (dados.cartas_na_mesa) {
        dados.cartas_na_mesa.forEach((carta, i) => {
            const el = document.createElement('div');
            const textoValor = traduzirValor(carta.valor);
            const simbolo = obterSimbolo(carta.naipe);
            el.className = `carta ${carta.naipe}`;
            el.innerHTML = `<span>${textoValor}</span>${gerarFigura(textoValor, carta.naipe)}`;
            el.setAttribute('data-naipe-simbolo', simbolo);
            const rotacoes = [-4, 3, -2, 5];
            el.style.setProperty('--rotacao-mesa', `${rotacoes[i % rotacoes.length]}deg`);
            mesaDiv.appendChild(el);
        });
    }

    // --- Oponentes ---
    ['cadeira-esquerda', 'cadeira-topo', 'cadeira-direita'].forEach(id => {
        document.getElementById(id).innerHTML = '';
    });

    if (dados.jogadores) {
        const total = dados.jogadores.length;
        dados.jogadores.forEach(j => {
            if (j.id === meuId) return;

            const pos = (j.id - meuId + total) % total;
            let idCadeira = '';
            if (total === 4) {
                if (pos === 1) idCadeira = 'cadeira-direita';
                else if (pos === 2) idCadeira = 'cadeira-topo';
                else if (pos === 3) idCadeira = 'cadeira-esquerda';
            } else if (total === 2) {
                idCadeira = 'cadeira-topo';
            }

            if (!idCadeira) return;

            const equipe = j.equipe || (j.id % 2 === 0 ? 1 : 2);
            const suaVez = dados.jogador_da_vez === j.id;
            const maoHtml = Array.from({ length: j.cartas_na_mao || 0 })
                .map(() => '<div class="carta-verso"></div>')
                .join('');

            document.getElementById(idCadeira).innerHTML = `
                <div class="perfil-jogador ${suaVez ? 'sua-vez' : ''}">
                    <h3>${j.nome || 'Jogador ' + j.id}</h3>
                    <span class="tag-equipe tag-equipe-${equipe}">Equipe ${equipe}</span>
                </div>
                <div class="mao-oponente">${maoHtml}</div>
            `;
        });

        // --- Mão do jogador local ---
        if (eu) {
            document.getElementById('local-equipe').innerText = eu.equipe || 1;
            document.getElementById('local-quedas').innerText = dados.quedas_eq1 !== undefined
                ? (eu.equipe === 1 ? dados.quedas_eq1 : dados.quedas_eq2)
                : 0;

            const minhaMaoDiv = document.getElementById('minha-mao');
            minhaMaoDiv.innerHTML = '';
            const ehMinhaVez = dados.jogador_da_vez === meuId;
            const totalCartas = eu.mao.length;
            const meio = (totalCartas - 1) / 2;

            eu.mao.forEach((carta, i) => {
                const el = document.createElement('div');
                const textoValor = traduzirValor(carta.valor);
                const simbolo = obterSimbolo(carta.naipe);
                const angulo = (i - meio) * 6;
                const transY = Math.abs(i - meio) * 4;

                el.style.setProperty('--rotacao', `${angulo}deg`);
                el.style.setProperty('--transY', `${transY}px`);
                el.className = `carta ${carta.naipe}${carta.manilha ? ' manilha' : ''}`;
                el.innerHTML = `<span>${textoValor}</span>${gerarFigura(textoValor, carta.naipe)}`;
                el.setAttribute('data-naipe-simbolo', simbolo);

                // Só pode jogar se for sua vez e não houver pedido de truco pendente
                if (ehMinhaVez && !dados.aguardando_resposta_truco) {
                    el.onmousedown = () => jogarCarta(i);
                } else {
                    el.classList.add('bloqueada');
                }

                minhaMaoDiv.appendChild(el);
            });
        }
    }

    // --- Painel de ações ---
    atualizarPainelAcoes(dados);

    // --- Modais de eventos ---
    if (dados.evento) {
        tratarEvento(dados.evento, dados);
    }

    // --- Animação de distribuição ---
    if (novaDistribuicao) {
        animarDistribuicao(dados);
    }
}

// ==========================================
// PAINEL DE AÇÕES (TRUCO E RESPOSTAS)
// ==========================================
function atualizarPainelAcoes(dados) {
    const painel = document.getElementById('painel-acoes');
    const titulo = document.getElementById('titulo-acao');
    const botoes = document.getElementById('botoes-acao');
    const ehMinhaVez = dados.jogador_da_vez === meuId;

    botoes.innerHTML = '';

    // Preciso responder a um pedido de truco?
    if (dados.aguardando_resposta_truco && dados.equipe_respondendo === obterMinhaEquipe(dados)) {
        titulo.innerText = `Pedido de ${dados.nome_nivel_truco}!`;
        painel.classList.remove('escondido');

        const btnAceitar = criarBotao('Aceitar', 'aceitar', () => responderTruco('ACEITAR'));
        const btnRecusar = criarBotao('Recusar (1pt)', 'recusar', () => responderTruco('RECUSAR'));
        botoes.appendChild(btnAceitar);
        botoes.appendChild(btnRecusar);

        // Pode aumentar?
        if (dados.nivel_truco < 4) {
            const nomes = ['', 'Truco', 'Seis', 'Nove', 'Doze'];
            const btnAumentar = criarBotao(nomes[dados.nivel_truco + 1] + '!', 'aumentar', () => responderTruco('AUMENTAR'));
            botoes.appendChild(btnAumentar);
        }

    } else if (ehMinhaVez && !dados.aguardando_resposta_truco && dados.nivel_truco < 4) {
        // É minha vez e posso pedir truco
        titulo.innerText = 'Sua vez:';
        painel.classList.remove('escondido');

        const nomes = ['', 'Truco', 'Seis', 'Nove', 'Doze'];
        const btnTruco = criarBotao(nomes[dados.nivel_truco + 1] + '!', 'truco', () => pedirTruco());
        botoes.appendChild(btnTruco);

    } else {
        painel.classList.add('escondido');
    }
}

function criarBotao(texto, classe, acao) {
    const btn = document.createElement('button');
    btn.className = `btn-acao ${classe}`;
    btn.innerText = texto;
    btn.onmousedown = () => { tocarSom('click.mp3'); acao(); };
    return btn;
}

function obterMinhaEquipe(dados) {
    if (!dados.jogadores) return 1;
    const eu = dados.jogadores.find(j => j.id === meuId);
    return eu ? eu.equipe : 1;
}

// ==========================================
// TRATAMENTO DE EVENTOS DO SERVIDOR
// ==========================================
function tratarEvento(evento, dados) {
    switch (evento) {
        case 'TRUCO_PEDIDO':
            mostrarModal(`<h2 style="color:#f0c040;">🃏 ${dados.nome_pedidor} pediu ${dados.nome_nivel_truco}!</h2><p>Mão pode valer ${dados.valor_se_aceito} pontos</p>`, 2000);
            tocarSom('click.mp3');
            break;
        case 'TRUCO_ACEITO':
            mostrarModal(`<h2 style="color:#4ade80;">✅ Truco aceito!</h2><p>Mão vale ${dados.valor_mao} pontos</p>`, 2000);
            break;
        case 'TRUCO_RECUSADO':
            mostrarModal(`<h2 style="color:#f87171;">❌ Truco recusado!</h2><p>Equipe ${dados.equipe_vencedora} ganha ${dados.pontos_ganhos} ponto(s)</p>`, 2500);
            break;
        case 'FIM_QUEDA':
            if (dados.vencedor_queda === 0) {
                mostrarModal(`<h2 style="color:#60a5fa;">🤝 Queda empatou!</h2>`, 1800);
            } else {
                mostrarModal(`<h2 style="color:#4ade80;">⚔️ Equipe ${dados.vencedor_queda} venceu a queda!</h2>`, 1800);
            }
            tocarSom('jogar_carta.ogg');
            break;
        case 'FIM_MAO':
            if (dados.vencedor_mao === 0) {
                mostrarModal(`<h2 style="color:#9ca3af;">🤝 Mão empatada! Ninguém pontua.</h2>`, 2500);
            } else {
                mostrarModal(`<h2 style="color:#f0c040;">🏆 Equipe ${dados.vencedor_mao} venceu a mão! +${dados.valor_mao}pt</h2>`, 2500);
                tocarSom('victory_6.mp3');
            }
            break;
        case 'FIM_PARTIDA':
            mostrarModal(`
                <h2 style="color:#f0c040;">🥇 EQUIPE ${dados.equipe_vencedora} VENCEU!</h2>
                <p>Placar final: ${dados.pontos_equipe1} × ${dados.pontos_equipe2}</p>
                <p style="margin-top:20px;"><a href="/pages/menu.html" style="color:#60a5fa;">Voltar ao Menu</a></p>
            `, 0);
            tocarSom('victory_6.mp3');
            break;
    }
}

// ==========================================
// AÇÕES DO JOGADOR
// ==========================================
function jogarCarta(indice) {
    tocarSom('jogar_carta.ogg');
    socket.send(JSON.stringify({
        acao: "JOGAR_CARTA",
        jogador_id: meuId,
        indice: indice
    }));
}

function pedirTruco() {
    socket.send(JSON.stringify({
        acao: "PEDIR_TRUCO",
        jogador_id: meuId
    }));
}

function responderTruco(resposta) {
    socket.send(JSON.stringify({
        acao: "RESPONDER_TRUCO",
        jogador_id: meuId,
        resposta: resposta   // "ACEITAR", "RECUSAR" ou "AUMENTAR"
    }));
}

// ==========================================
// FUNÇÕES AUXILIARES
// ==========================================
function traduzirValor(valor) {
    const mapa = { 1: "A", 11: "J", 12: "Q", 13: "K" };
    return mapa[valor] || valor;
}

function obterSimbolo(naipe) {
    const simbolos = { paus: "♣", copas: "♥", espadas: "♠", ouros: "♦" };
    return simbolos[naipe] || "";
}

function gerarFigura(textoValor, naipe) {
    const figuras = { K: "rei.png", Q: "rainha.png", J: "valete.png" };
    const arquivo = figuras[textoValor];
    if (!arquivo) return "";
    const vermelho = (naipe === "copas" || naipe === "ouros") ? " filtro-vermelho" : "";
    return `<img src="/assets/cartas/${arquivo}" class="figura-centro${vermelho}" alt="${textoValor}">`;
}

function mostrarModal(htmlContent, tempoMs) {
    const modal = document.getElementById('modal-notificacao');
    const texto = document.getElementById('modal-texto');
    texto.innerHTML = htmlContent;
    modal.classList.remove('modal-oculto');
    if (window.modalTimer) clearTimeout(window.modalTimer);
    if (tempoMs > 0) {
        window.modalTimer = setTimeout(() => modal.classList.add('modal-oculto'), tempoMs);
    }
}

function animarDistribuicao(dados) {
    tocarSom('shuffle.mp3');
    const baralhoEl = document.querySelector('.centro-da-mesa');
    if (!baralhoEl) return;
    const rect = baralhoEl.getBoundingClientRect();
    const origemX = rect.left + rect.width / 2;
    const origemY = rect.top + rect.height / 2;
    let delay = 0;

    if (!dados.jogadores) return;
    dados.jogadores.forEach(j => {
        let idAlvo = 'cadeira-base';
        if (j.id !== meuId) {
            const total = dados.jogadores.length;
            const pos = (j.id - meuId + total) % total;
            if (total === 4) {
                if (pos === 1) idAlvo = 'cadeira-direita';
                else if (pos === 2) idAlvo = 'cadeira-topo';
                else if (pos === 3) idAlvo = 'cadeira-esquerda';
            } else if (total === 2) {
                idAlvo = 'cadeira-topo';
            }
        }

        const assento = document.getElementById(idAlvo);
        if (!assento) return;
        const rectAlvo = assento.getBoundingClientRect();
        const destX = rectAlvo.left + rectAlvo.width / 2;
        const destY = rectAlvo.top + rectAlvo.height / 2;
        const qtd = j.cartas_na_mao || (j.mao ? j.mao.length : 3);

        for (let c = 0; c < qtd; c++) {
            setTimeout(() => {
                const cartaVoadora = document.createElement('div');
                cartaVoadora.className = 'carta-verso carta-animada';
                cartaVoadora.style.left = origemX + 'px';
                cartaVoadora.style.top = origemY + 'px';
                cartaVoadora.style.width = '40px';
                cartaVoadora.style.height = '60px';
                cartaVoadora.style.transform = 'translate(-50%, -50%) scale(1)';
                document.body.appendChild(cartaVoadora);
                requestAnimationFrame(() => {
                    cartaVoadora.style.left = destX + 'px';
                    cartaVoadora.style.top = destY + 'px';
                    cartaVoadora.style.transform = 'translate(-50%, -50%) scale(0.5) rotate(720deg)';
                    cartaVoadora.style.opacity = '0';
                });
                setTimeout(() => cartaVoadora.remove(), 600);
            }, delay);
            delay += 80;
        }
    });
}
