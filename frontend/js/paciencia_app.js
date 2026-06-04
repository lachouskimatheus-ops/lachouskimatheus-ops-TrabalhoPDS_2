// ==========================================
// CONEXÃO WEBSOCKET COM O C++
// ==========================================
const socket = new WebSocket('ws://localhost:8080/ws');

socket.onopen = () => {
    console.log('Conectado ao servidor C++');
    socket.send(JSON.stringify({ acao: 'OBTER_ESTADO_ATUAL' }));
    iniciarCronometro();
};

socket.onmessage = (event) => {
    const estado = JSON.parse(event.data);
    window.estadoAtual = estado;
    atualizarInterface(estado);
};

socket.onerror = () => {
    mostrarModal('<h2 style="color:#ef4444">⚠ Servidor offline</h2><p>Verifique se o servidor C++ está rodando.</p>', 5000);
};


// ==========================================
// CRONÔMETRO
// ==========================================
let timerInterval;
let tempoSegundos = 0;

function iniciarCronometro() {
    clearInterval(timerInterval); // Garante que não criaremos intervalos duplicados
    
    // NÃO ZERE O TEMPO AQUI!
    // Esta função apenas retoma ou inicia a contagem.
    timerInterval = setInterval(() => {
        tempoSegundos++;
        atualizarVisorCronometro();
    }, 1000);
}

function pararCronometro() {
    clearInterval(timerInterval); // congela o tempo
}

function zerarCronometro() {
    tempoSegundos = 0; // Só chame isso quando for um jogo novo
    atualizarVisorCronometro();
}

function atualizarVisorCronometro() {
    const min = String(Math.floor(tempoSegundos / 60)).padStart(2, '0');
    const seg = String(tempoSegundos % 60).padStart(2, '0');
    const el = document.getElementById('cronometro-visor');
    if(el) el.innerText = `${min}:${seg}`;
}

// Função auxiliar para o botão "Jogar Novamente" do modal
window.jogarNovamenteModal = function() {
    socket.send(JSON.stringify({ acao: 'NOVO_JOGO' }));
    document.getElementById('modal-notificacao').classList.add('modal-oculto');
    cartaSelecionada = null;
    iniciarCronometro();
};

// ==========================================
// ESTADO LOCAL DE SELEÇÃO
// ==========================================
// cartaSelecionada agora pode guardar 'cartaIdx' para identificar onde o bloco começa
let cartaSelecionada = null; // { tipo: "coluna"|"descarte"|"fundacao", indice: int, cartaIdx: int }
let jogoPausado = false;
// ==========================================
// ATUALIZAR INTERFACE COM BASE NO ESTADO
// ==========================================
function atualizarInterface(estado) {
    // Pontuação
    document.getElementById('pontos').innerText   = estado.pontuacao || 0;
    document.getElementById('recorde').innerText  = estado.recorde   || 0;

    // Cava
    const cavaCount = estado.cava_tamanho || 0;
    document.getElementById('cava-count').innerText = cavaCount;
    const cavaVisual = document.getElementById('cava-visual');
    if (cavaCount === 0) {
        if (cavaVisual) cavaVisual.outerHTML = '<div class="cava-vazia" id="cava-visual" onclick="comprarCarta()">↺</div>';
    } else {
        const el = document.getElementById('cava-visual');
        if (el) el.className = 'verso-grande';
    }

        // Dentro da sua função atualizarInterface(estado)
    if (estado.vitoria) {
        pararCronometro();
        const tempoFinal = document.getElementById('cronometro-visor')?.innerText || "00:00";
        const pontos = estado.pontuacao || 0;
        const recorde = estado.recorde || 0;
        
        const modalHtml = `
            <div style="text-align: center; padding: 10px;">
                <h2 style="color:#4ade80; margin-bottom: 10px; font-size: 2em; font-family: 'Cinzel Decorative', serif;">🏆 PARABÉNS!</h2>
                <p style="margin-bottom: 15px; font-size: 1.2em; color: #fff;">Você completou o Paciência!</p>
                
                <div style="background: rgba(0,0,0,0.5); border: 1px solid rgba(255, 200, 50, 0.2); border-radius: 8px; padding: 15px; margin-bottom: 20px; color: #fff;">
                    <p style="margin: 5px 0;">Tempo: <strong style="color: #f0c040;">${tempoFinal}</strong></p>
                    <p style="margin: 5px 0;">Pontuação Final: <strong style="color: #f0c040;">${pontos}</strong></p>
                    <p style="margin: 5px 0;">Recorde Atual: <strong style="color: #f0c040;">${recorde}</strong></p>
                </div>

                <div style="display: flex; gap: 15px; justify-content: center; flex-wrap: wrap;">
                    <button onclick="window.location.href='menu.html'" style="padding: 12px 20px; cursor: pointer; background: rgba(255,255,255,0.1); color: #fff; border: 1px solid rgba(255,255,255,0.2); border-radius: 5px; font-family: 'Cinzel', serif;">VOLTAR AO MENU</button>
                    <button onclick="jogarNovamenteModal()" style="padding: 12px 20px; cursor: pointer; background: rgba(240,192,64,0.2); color: #f0c040; border: 1px solid rgba(240,192,64,0.5); border-radius: 5px; font-weight: bold; font-family: 'Cinzel', serif;">JOGAR NOVAMENTE</button>
                </div>
            </div>
        `;
        
        // Insere o HTML dentro da div modal-texto que você já tem no paciencia.html
        document.getElementById('modal-texto').innerHTML = modalHtml;
        document.getElementById('modal-notificacao').classList.remove('modal-oculto');
    }

    // Descarte
    renderizarDescarte(estado.descarte);

    // Fundações
    renderizarFundacoes(estado.fundacoes);

    // Colunas
    renderizarColunas(estado.colunas, estado.cartas_escondidas);

    // Vitória
    if (estado.vitoria) {
        mostrarModal('<h2 style="color:#4ade80">🏆 PARABÉNS!</h2><p>Você completou o Paciência!</p>', 0);
    }
}

// ==========================================
// RENDERIZAR DESCARTE
// ==========================================
function renderizarDescarte(descarte) {
    const div = document.getElementById('descarte');
    div.innerHTML = '';

    if (!descarte || descarte.length === 0) {
        div.innerHTML = '<span class="placeholder">↩</span>';
        return;
    }

    const carta = descarte[descarte.length - 1];
    const el    = criarElementoCarta(carta, 'descarte');
    el.classList.add('carta-descarte');

    // Lógica Drag and Drop
    el.draggable = true;
    el.ondragstart = (e) => {
        e.dataTransfer.setData('app/jogo', JSON.stringify({ tipo: 'descarte', indice: 0 }));
    };

    const selecionado = cartaSelecionada && cartaSelecionada.tipo === 'descarte';
    if (selecionado) el.classList.add('selecionada');

    el.onclick = (e) => {
        e.stopPropagation();
        selecionarDescarte();
    };
    div.appendChild(el);
}

// ==========================================
// RENDERIZAR FUNDAÇÕES
// ==========================================
function renderizarFundacoes(fundacoes) {
    for (let i = 0; i < 4; i++) {
        const div = document.getElementById(`fundacao-${i}`);
        div.innerHTML = '';
        
        // Configura a área de fundação para aceitar "soltar" cartas
        div.ondragover = (e) => e.preventDefault(); 
        div.ondrop = (e) => processarDrop(e, 'fundacao', i);

        const simbolos = ['♣', '♥', '♠', '♦'];
        if (!fundacoes || !fundacoes[i] || fundacoes[i].length === 0) {
            div.innerHTML = `<span class="placeholder">${simbolos[i]}</span>`;
        } else {
            const carta = fundacoes[i][fundacoes[i].length - 1];
            const el    = criarElementoCarta(carta, 'fundacao');
            el.classList.add('carta-coluna', 'frente');
            el.style.position = 'relative';
            
            // Permite puxar da fundação
            el.draggable = true;
            el.ondragstart = (event) => {
                event.dataTransfer.setData('app/jogo', JSON.stringify({ tipo: 'fundacao', indice: i }));
            };

            const selecionado = cartaSelecionada && cartaSelecionada.tipo === 'fundacao' && cartaSelecionada.indice === i;
            if (selecionado) el.classList.add('selecionada');

            div.appendChild(el);
        }

        div.onclick = (e) => {
            e.stopPropagation();
            if (cartaSelecionada) {
                moverParaDestino('fundacao', i);
            } else if (fundacoes && fundacoes[i] && fundacoes[i].length > 0) {
                selecionarFundacao(i);
            }
        };
    }
}

// ==========================================
// RENDERIZAR COLUNAS
// ==========================================
function renderizarColunas(colunas, cartasEscondidas) {
    for (let i = 0; i < 7; i++) {
        const div = document.getElementById(`coluna-${i}`);
        div.innerHTML = '';
        
        // Área para soltar cartas
        div.ondragover = (e) => e.preventDefault();
        div.ondrop = (e) => processarDrop(e, 'coluna', i);

        const coluna     = colunas ? colunas[i] : [];
        const escondidas = cartasEscondidas ? cartasEscondidas[i] : 0;
        const offsetY    = 25; 

        coluna.forEach((carta, j) => {
            const el = document.createElement('div');
            el.classList.add('carta-coluna');
            el.style.top = `${j * offsetY}px`;
            el.style.zIndex = j + 1;
            
            if (j < escondidas) {
                el.classList.add('verso');
            } else {
                const naipe   = obterNomeNaipe(carta.naipe);
                const simbolo = obterSimboloNaipe(carta.naipe);
                const valor   = traduzirValor(carta.valor);

                el.classList.add('frente', naipe);
                el.setAttribute('data-naipe-simbolo', simbolo);
                el.innerText = valor;

                // Transforma cada carta virada para cima em "arrastável"
                el.draggable = true;
                el.ondragstart = (event) => {
                    event.dataTransfer.setData('app/jogo', JSON.stringify({ tipo: 'coluna', indice: i, cartaIdx: j }));
                };

                const colunaSelecionada = cartaSelecionada && cartaSelecionada.tipo === 'coluna' && cartaSelecionada.indice === i;
                if (colunaSelecionada && j >= cartaSelecionada.cartaIdx) {
                    el.classList.add('selecionada');
                }

                el.onclick = (e) => { 
                    e.stopPropagation(); 
                    if (cartaSelecionada && !(cartaSelecionada.tipo === 'coluna' && cartaSelecionada.indice === i)) {
                        moverParaDestino('coluna', i);
                    } else {
                        selecionarColuna(i, j);
                    }
                };
            }
            div.appendChild(el);
        });

        div.style.minHeight = `${Math.max(110, coluna.length * offsetY + 105)}px`;
        
        div.onclick = (e) => {
            e.stopPropagation();
            if (cartaSelecionada) moverParaDestino('coluna', i);
        };
    }
}

// ==========================================
// SELEÇÃO E MOVIMENTO
// ==========================================
function selecionarDescarte() {
    if (cartaSelecionada && cartaSelecionada.tipo === 'descarte') {
        cartaSelecionada = null;
    } else {
        cartaSelecionada = { tipo: 'descarte', indice: 0 };
    }
    renderizarDescarte(window.estadoAtual?.descarte);
    renderizarColunas(window.estadoAtual?.colunas, window.estadoAtual?.cartas_escondidas);
    renderizarFundacoes(window.estadoAtual?.fundacoes);
}

function selecionarColuna(colunaIdx, cartaIdx) {
    if (cartaSelecionada && 
        cartaSelecionada.tipo   === 'coluna' && 
        cartaSelecionada.indice === colunaIdx && 
        cartaSelecionada.cartaIdx === cartaIdx) {
        // Se clicar exatamente na mesma carta que já estava selecionada, cancela
        cartaSelecionada = null;
    } else {
        // Seleciona o bloco a partir do índice clicado (cartaIdx)
        cartaSelecionada = { tipo: 'coluna', indice: colunaIdx, cartaIdx: cartaIdx };
    }
    renderizarDescarte(window.estadoAtual?.descarte);
    renderizarColunas(window.estadoAtual?.colunas, window.estadoAtual?.cartas_escondidas);
    renderizarFundacoes(window.estadoAtual?.fundacoes);
}

function selecionarFundacao(fundacaoIdx) {
    if (cartaSelecionada && cartaSelecionada.tipo === 'fundacao' && cartaSelecionada.indice === fundacaoIdx) {
        cartaSelecionada = null;
    } else {
        cartaSelecionada = { tipo: 'fundacao', indice: fundacaoIdx };
    }
    renderizarDescarte(window.estadoAtual?.descarte);
    renderizarColunas(window.estadoAtual?.colunas, window.estadoAtual?.cartas_escondidas);
    renderizarFundacoes(window.estadoAtual?.fundacoes);
}

function moverParaDestino(tipoDestino, indiceDestino) {
    if (!cartaSelecionada) return;

    let payload = {};

    // 1. Se a origem for a FUNDAÇÃO
    if (cartaSelecionada.tipo === 'fundacao') {
        payload = {
            acao: 'MOVER_DA_FUNDACAO',
            fundacao_indice: cartaSelecionada.indice,
            destino_tipo: tipoDestino,
            destino_indice: indiceDestino
        };
    }
    // 2. Se a origem for uma COLUNA e clicamos no meio do bloco (ou seja, mover mais de uma carta ou uma específica)
    else if (cartaSelecionada.tipo === 'coluna') {
        const totalCartasColuna = window.estadoAtual?.colunas[cartaSelecionada.indice]?.length || 0;
        
        // Se a carta selecionada não for a última da coluna, é um movimento de Bloco obrigatório.
        // Se for a última, o backend pode tratar tanto como MOVER normal ou MOVER_BLOCO com tamanho 1.
        if (cartaSelecionada.cartaIdx < totalCartasColuna - 1 || tipoDestino === 'coluna') {
            payload = {
                acao: 'MOVER_BLOCO',
                origem_coluna: cartaSelecionada.indice,
                carta_idx: cartaSelecionada.cartaIdx,
                destino_coluna: indiceDestino
            };
        } else {
            // Movimento simples (ex: última carta da coluna indo para a Fundação)
            payload = {
                acao: 'MOVER',
                origem_tipo: cartaSelecionada.tipo,
                origem_indice: cartaSelecionada.indice,
                destino_tipo: tipoDestino,
                destino_indice: indiceDestino
            };
        }
    } 
    // 3. Movimentos vindos do descarte
    else {
        payload = {
            acao: 'MOVER',
            origem_tipo: cartaSelecionada.tipo,
            origem_indice: cartaSelecionada.indice,
            destino_tipo: tipoDestino,
            destino_indice: indiceDestino
        };
    }

    socket.send(JSON.stringify(payload));
    cartaSelecionada = null;
}

function comprarCarta() {
    socket.send(JSON.stringify({ acao: 'COMPRAR_CARTA' }));
}

// ==========================================
// BOTÕES
// ==========================================
document.getElementById('btn-desfazer').onclick = () => {
    socket.send(JSON.stringify({ acao: 'DESFAZER' }));
    cartaSelecionada = null;
};

document.getElementById('btn-novo-jogo').onclick = () => {
    socket.send(JSON.stringify({ acao: 'NOVO_JOGO' }));
    cartaSelecionada = null;
    zerarCronometro();
    iniciarCronometro();
};

// ==========================================
// ATALHOS DE TECLADO
// ==========================================
document.addEventListener('keydown', (e) => {
    // Atalho ESC: Pausar/Retomar
    if (e.key === 'Escape') {
        alternarPausa();
    }
    // Atalho Ctrl + Z: Desfazer
    if (e.ctrlKey && e.key === 'z') {
        document.getElementById('btn-desfazer').click();
    }
});
// ==========================================
// AUXILIARES
// ==========================================
function criarElementoCarta(carta, tipo) {
    const el     = document.createElement('div');
    const naipe  = obterNomeNaipe(carta.naipe);
    const simbolo = obterSimboloNaipe(carta.naipe);
    const valor  = traduzirValor(carta.valor);

    el.classList.add(naipe);
    el.setAttribute('data-naipe-simbolo', simbolo);
    el.innerText = valor;
    return el;
}

function traduzirValor(valor) {
    const mapa = { 1: 'A', 11: 'J', 12: 'Q', 13: 'K' };
    return mapa[valor] || valor;
}

function obterNomeNaipe(naipe) {
    const mapa = { 0: 'paus', 1: 'copas', 2: 'espadas', 3: 'ouros' };
    return mapa[naipe] || naipe;
}

function obterSimboloNaipe(naipe) {
    const mapa = { 0: '♣', 1: '♥', 2: '♠', 3: '♦' };
    return mapa[naipe] || '';
}

function mostrarModal(htmlContent, tempoMs) {
    const modal = document.getElementById('modal-notificacao');
    const texto = document.getElementById('modal-texto');

    texto.innerHTML = htmlContent;
    modal.classList.remove('modal-oculto');

    if (tempoMs > 0) {
        if (window.modalTimer) clearTimeout(window.modalTimer);
        window.modalTimer = setTimeout(() => {
            modal.classList.add('modal-oculto');
        }, tempoMs);
    }
}

// ==========================================
// LÓGICA DE PAUSA E REGRAS
// ==========================================

function alternarPausa() {
    const modal = document.getElementById('modal-notificacao');
    
    if (!jogoPausado) {
        pararCronometro(); // Certifique-se de que essa função existe
        jogoPausado = true;
        document.getElementById('modal-texto').innerHTML = `
            <h2>JOGO PAUSADO</h2>
            <div style="margin: 20px 0;">
                <button onclick="alternarPausa()" style="width:100%; padding:10px; margin-bottom:10px; background:#4ade80; border:none; cursor:pointer;">Retomar Jogo</button>
                <button onclick="window.location.href='menu.html'" style="width:100%; padding:10px; background:#374151; border:none; color:white; cursor:pointer;">Voltar ao Menu</button>
            </div>
        `;
        modal.classList.remove('modal-oculto');
    } else {
        iniciarCronometro(); // Certifique-se de que essa função existe
        jogoPausado = false;
        modal.classList.add('modal-oculto');
    }
}

function mostrarRegras() {
    pararCronometro();
    document.getElementById('modal-texto').innerHTML = `
        <div class="regras-box">
            <h3>COMO JOGAR PACIÊNCIA</h3>
            <ul class="regras-lista" style="text-align: left; margin: 15px 0;">
                <li><strong>Objetivo:</strong> Mova todas as cartas para as fundações (topo) por naipe.</li>
                <li><strong>Colunas:</strong> Ordene as cartas de forma decrescente com cores alternadas.</li>
                <li><strong>Atalhos:</strong> <b>ESC</b> para pausar e <b>Ctrl+Z</b> para desfazer.</li>
                <li><strong>Arrastar:</strong> Clique nas cartas para selecionar e mover.</li>
            </ul>
        </div>
        <button onclick="fecharModal()" style="margin-top:15px; padding:10px 20px; background:#4ade80; border:none; cursor:pointer;">Entendido</button>
    `;
    document.getElementById('modal-notificacao').classList.remove('modal-oculto');
}

function fecharModal() {
    document.getElementById('modal-notificacao').classList.add('modal-oculto');
    if (!jogoPausado) iniciarCronometro();
}

window.jogarNovamenteModal = function() {
    socket.send(JSON.stringify({ acao: 'NOVO_JOGO' }));
    document.getElementById('modal-notificacao').classList.add('modal-oculto');
    cartaSelecionada = null;
    iniciarCronometro();
};

function processarDrop(event, tipoDestino, indiceDestino) {
    event.preventDefault();
    const origemRaw = event.dataTransfer.getData('app/jogo');
    if (!origemRaw) return;
    
    const origem = JSON.parse(origemRaw);
    let payload = {};

    if (origem.tipo === 'fundacao') {
        payload = {
            acao: 'MOVER_DA_FUNDACAO',
            fundacao_indice: origem.indice,
            destino_tipo: tipoDestino,
            destino_indice: indiceDestino
        };
    } else if (origem.tipo === 'coluna') {
        const totalCartasColuna = window.estadoAtual?.colunas[origem.indice]?.length || 0;
        
        if (origem.cartaIdx < totalCartasColuna - 1 || tipoDestino === 'coluna') {
            payload = {
                acao: 'MOVER_BLOCO',
                origem_coluna: origem.indice,
                carta_idx: origem.cartaIdx,
                destino_coluna: indiceDestino
            };
        } else {
            payload = {
                acao: 'MOVER',
                origem_tipo: origem.tipo,
                origem_indice: origem.indice,
                destino_tipo: tipoDestino,
                destino_indice: indiceDestino
            };
        }
    } else {
        payload = {
            acao: 'MOVER',
            origem_tipo: origem.tipo,
            origem_indice: origem.indice,
            destino_tipo: tipoDestino,
            destino_indice: indiceDestino
        };
    }

    socket.send(JSON.stringify(payload));
    cartaSelecionada = null; // Limpa a seleção visual (clique) se o jogador usou drag & drop
}