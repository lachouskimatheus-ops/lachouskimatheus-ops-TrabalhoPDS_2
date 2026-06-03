// ==========================================
// CONEXÃO WEBSOCKET COM O C++
// ==========================================
const socket = new WebSocket('ws://localhost:8080/ws');

socket.onopen = () => {
    console.log('Conectado ao servidor C++');
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
// ESTADO LOCAL DE SELEÇÃO
// ==========================================
// cartaSelecionada agora pode guardar 'cartaIdx' para identificar onde o bloco começa
let cartaSelecionada = null; // { tipo: "coluna"|"descarte"|"fundacao", indice: int, cartaIdx: int }

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

        const simbolos = ['♣', '♥', '♠', '♦'];
        if (!fundacoes || !fundacoes[i] || fundacoes[i].length === 0) {
            div.innerHTML = `<span class="placeholder">${simbolos[i]}</span>`;
        } else {
            const carta = fundacoes[i][fundacoes[i].length - 1];
            const el    = criarElementoCarta(carta, 'fundacao');
            el.classList.add('carta-coluna', 'frente');
            el.style.position = 'relative';
            
            // Nova lógica: destacar se a fundação for selecionada como origem
            const selecionado = cartaSelecionada && 
                                cartaSelecionada.tipo === 'fundacao' && 
                                cartaSelecionada.indice === i;
            if (selecionado) el.classList.add('selecionada');

            div.appendChild(el);
        }

        // Modificado para alternar entre selecionar a fundação ou receber uma carta
        div.onclick = (e) => {
            e.stopPropagation();
            if (cartaSelecionada) {
                // Se já temos algo selecionado, tenta mover para cá
                moverParaDestino('fundacao', i);
            } else if (fundacoes && fundacoes[i] && fundacoes[i].length > 0) {
                // Se não tem seleção e a pilha tem cartas, seleciona o topo da fundação
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
        const div     = document.getElementById(`coluna-${i}`);
        div.innerHTML = '';

        const coluna    = colunas ? colunas[i] : [];
        const escondidas = cartasEscondidas ? cartasEscondidas[i] : 0;
        const offsetY   = 25; 

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

                // Nova lógica de seleção visual de blocos:
                // Se a coluna atual está selecionada e o clique foi nesta carta ou em alguma acima dela (bloco)
                const colunaSelecionada = cartaSelecionada && 
                                          cartaSelecionada.tipo === 'coluna' && 
                                          cartaSelecionada.indice === i;
                
                if (colunaSelecionada && j >= cartaSelecionada.cartaIdx) {
                    el.classList.add('selecionada');
                }

                // Qualquer carta aberta agora é clicável para iniciar um bloco!
                el.onclick = (e) => { 
                    e.stopPropagation(); 
                    if (cartaSelecionada && !(cartaSelecionada.tipo === 'coluna' && cartaSelecionada.indice === i)) {
                        // Se já tem seleção vinda de OUTRA coluna/lugar, tenta mover para esta coluna
                        moverParaDestino('coluna', i);
                    } else {
                        // Caso contrário, seleciona a partir desta carta específica para mover o bloco
                        selecionarColuna(i, j);
                    }
                };
            }

            div.appendChild(el);
        });

        div.style.minHeight = `${Math.max(110, coluna.length * offsetY + 105)}px`;
        
        // Clique no fundo vazio da coluna
        div.onclick = (e) => {
            e.stopPropagation();
            if (cartaSelecionada) {
                moverParaDestino('coluna', i);
            }
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
};

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