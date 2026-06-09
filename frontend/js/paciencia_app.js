// ==========================================
// CONEXÃO WEBSOCKET COM O C++
// ==========================================
const socket = new WebSocket('ws://localhost:8080/ws');

let estaProcessando = false; //Necessario para Travar os cliques

socket.onclose = () => {
    console.warn('Conexão perdida. Tentando reconectar em 3 segundos...');
    setTimeout(() => {
        // Tenta recarregar a conexão ou mostrar um aviso ao jogador
        location.reload(); 
    }, 3000);
};

// Adicione isso logo após abrir o socket
socket.onopen = () => {
    console.log('Conectado ao servidor C++');
    socket.send(JSON.stringify({ acao: 'OBTER_ESTADO_ATUAL' }));
    iniciarCronometro();

    // MANTÉM A CONEXÃO VIVA
    setInterval(() => {
        if (socket.readyState === WebSocket.OPEN) {
            socket.send(JSON.stringify({ acao: 'PING' }));
        }
    }, 30000); 
};

socket.onmessage = (event) => {
    const estado = JSON.parse(event.data);
    
    // Se o C++ enviar uma mensagem de que não houve movimento possível
    if (estado.movimento_realizado === false) {
        pararAutoCompletar();
        return;
    }

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
    vitoriaJaProcessada = false;
    zerarCronometro();
    iniciarCronometro();
};

// Função para checar regras básicas antes de mover
function ehMovimentoValido(origem, tipoDestino, indiceDestino) {
    const estado = window.estadoAtual;
    
    // 1. Identificar qual carta está sendo movida
    let cartaSendoMovida;
    if (origem.tipo === 'coluna') {
        cartaSendoMovida = estado.colunas[origem.indice][origem.cartaIdx];
    } else if (origem.tipo === 'descarte') {
        cartaSendoMovida = estado.descarte[estado.descarte.length - 1];
    } else {
        return true; // Se for movimento de fundação para fundação, o C++ já deve cuidar
    }

    // 2. Validação para FUNDAÇÃO
    if (tipoDestino === 'fundacao') {
        const fundacaoDestino = estado.fundacoes[indiceDestino];
        
        // Se a fundação está vazia, só aceita Ás (valor 1)
        if (!fundacaoDestino || fundacaoDestino.length === 0) {
            return cartaSendoMovida.valor === 1; 
        }
        
        // Se já tem carta, precisa ser o mesmo naipe e valor superior (n+1)
        const topoFundacao = fundacaoDestino[fundacaoDestino.length - 1];
        const mesmoNaipe = (cartaSendoMovida.naipe === topoFundacao.naipe);
        const sequenciaCerta = (cartaSendoMovida.valor === topoFundacao.valor + 1);
        
        return mesmoNaipe && sequenciaCerta;
    }

    // 3. Validação para COLUNA (o que você já tinha)
    if (tipoDestino === 'coluna') {
        const colunaDestino = estado.colunas[indiceDestino];
        if (!colunaDestino || colunaDestino.length === 0) return true;

        const cartaNoTopo = colunaDestino[colunaDestino.length - 1];
        
        // Regra de cores alternadas
        const ehVermelho = (c) => c.naipe === 1 || c.naipe === 3;
        if (ehVermelho(cartaSendoMovida) === ehVermelho(cartaNoTopo)) {
            return false;
        }
    }
    
    return true;
}

function tocarSom(nomeArquivo) {
    const audio = new Audio(`/assets/sons/${nomeArquivo}`); 
    
    audio.play().catch(e => {
        console.error("Erro ao carregar o som:", e);
    });
}
// ==========================================
// ESTADO LOCAL DE SELEÇÃO
// ==========================================
// cartaSelecionada agora pode guardar 'cartaIdx' para identificar onde o bloco começa
let cartaSelecionada = null; // { tipo: "coluna"|"descarte"|"fundacao", indice: int, cartaIdx: int }
let jogoPausado = false;
let autoCompletarDisponivel = false;
let vitoriaJaProcessada = false;
// ==========================================
// ATUALIZAR INTERFACE COM BASE NO ESTADO
// ==========================================
function atualizarInterface(estado) {
    // 1. Atualizar informações básicas
    document.getElementById('pontos').innerText = estado.pontuacao || 0;
    document.getElementById('recorde').innerText = estado.recorde || 0;


    const containerAuto = document.getElementById('container-auto');
if (containerAuto) {
    // Verifica se todas as cartas ocultas foram reveladas (total = 0)
    const totalEscondidas = estado.cartas_escondidas.reduce((a, b) => a + b, 0);
    
    if (totalEscondidas === 0 && !estado.vitoria) {
        containerAuto.style.display = 'block'; // Mostra o botão
    } else {
        containerAuto.style.display = 'none';  // Esconde o botão
    }
}
    // 2. Lógica de Vitória (Única e consolidada)
    if (estado.vitoria) {
        const modal = document.getElementById('modal-notificacao');
        
        // Verifica se já não está visível para não repetir o som/modal
        if (!vitoriaJaProcessada) {
            vitoriaJaProcessada = true;
            tocarSom('victory_6.mp3');
            pararCronometro();
            
            const tempoFinal = document.getElementById('cronometro-visor')?.innerText || "00:00";
            const pontos = estado.pontuacao || 0;
            const recorde = estado.recorde || 0;
            
            const novoRecorde = pontos > recorde;
            const recordeExibido = novoRecorde ? pontos : recorde;

            const modalHtml = `
                <div style="text-align: center; padding: 10px;">
                    <h2 style="color:#4ade80; margin-bottom: 10px; font-size: 2em; font-family: 'Cinzel Decorative', serif;">🏆 PARABÉNS!</h2>
                    <p style="margin-bottom: 15px; font-size: 1.2em; color: #fff;">Você completou o Paciência!</p>
                    
                    ${novoRecorde ? '<p style="color:#f0c040; font-size:1.1em; margin-bottom:10px;">⭐ Novo recorde!</p>' : ''}

                    <div style="background: rgba(0,0,0,0.5); border: 1px solid rgba(255, 200, 50, 0.2); border-radius: 8px; padding: 15px; margin-bottom: 20px; color: #fff;">
                        <p style="margin: 5px 0;">Tempo: <strong style="color: #f0c040;">${tempoFinal}</strong></p>
                        <p style="margin: 5px 0;">Pontuação Final: <strong style="color: #f0c040;">${pontos}</strong></p>
                        <p style="margin: 5px 0;">Recorde: <strong style="color: #f0c040;">${recordeExibido}</strong></p>
                    </div>

                    <div style="display: flex; gap: 15px; justify-content: center; flex-wrap: wrap;">
                        <button onclick="window.location.href='menu.html'" style="padding: 12px 20px; cursor: pointer; background: rgba(255,255,255,0.1); color: #fff; border: 1px solid rgba(255,255,255,0.2); border-radius: 5px; font-family: 'Cinzel', serif;">VOLTAR AO MENU</button>
                        <button onclick="mostrarRecordeModal()" style="padding: 12px 20px; cursor: pointer; background: rgba(74,222,128,0.15); color: #4ade80; border: 1px solid rgba(74,222,128,0.4); border-radius: 5px; font-family: 'Cinzel', serif;">VER RECORDE</button>
                        <button onclick="jogarNovamenteModal()" style="padding: 12px 20px; cursor: pointer; background: rgba(240,192,64,0.2); color: #f0c040; border: 1px solid rgba(240,192,64,0.5); border-radius: 5px; font-weight: bold; font-family: 'Cinzel', serif;">JOGAR NOVAMENTE</button>
                    </div>
                </div>
            `;
            document.getElementById('modal-texto').innerHTML = modalHtml;
            modal.classList.remove('modal-oculto');
        }
    }

    // 3. Atualizar Cava
    const cavaCount = estado.cava_tamanho || 0;
    document.getElementById('cava-count').innerText = cavaCount;
    const cavaVisual = document.getElementById('cava-visual');
    if (cavaCount === 0) {
        if (cavaVisual) cavaVisual.outerHTML = '<div class="cava-vazia" id="cava-visual" onclick="comprarCarta()">↺</div>';
    } else {
        const el = document.getElementById('cava-visual');
        if (el) el.className = 'verso-grande';
    }

    // 4. Renderizar o resto do tabuleiro
    renderizarDescarte(estado.descarte);
    renderizarFundacoes(estado.fundacoes);
    renderizarColunas(estado.colunas, estado.cartas_escondidas);
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
    // 1. Bloqueio: se já estiver processando, ignora novos cliques
    if (estaProcessando) {
        console.warn("Bloqueado: já processando movimento...");
        return;
    }

    // 2. Trava o sistema
    estaProcessando = true;

    // 3. Efeitos sonoros
    if (tipoDestino === 'fundacao') tocarSom('card-slide-1.ogg');
    else if (tipoDestino === 'coluna') tocarSom('card-place-3.ogg');

    let payload = {};

    // 4. Montagem do objeto de ação (Payload)
    if (cartaSelecionada.tipo === 'fundacao') {
        payload = {
            acao: 'MOVER_DA_FUNDACAO',
            fundacao_indice: cartaSelecionada.indice,
            destino_tipo: tipoDestino,
            destino_indice: indiceDestino
        };
    } else if (cartaSelecionada.tipo === 'coluna') {
        const totalCartasColuna = window.estadoAtual?.colunas[cartaSelecionada.indice]?.length || 0;
        
        if (cartaSelecionada.cartaIdx < totalCartasColuna - 1 || tipoDestino === 'coluna') {
            payload = {
                acao: 'MOVER_BLOCO',
                origem_coluna: cartaSelecionada.indice,
                carta_idx: cartaSelecionada.cartaIdx,
                destino_coluna: indiceDestino
            };
        } else {
            payload = {
                acao: 'MOVER',
                origem_tipo: cartaSelecionada.tipo,
                origem_indice: cartaSelecionada.indice,
                destino_tipo: tipoDestino,
                destino_indice: indiceDestino
            };
        }
    } else {
        payload = {
            acao: 'MOVER',
            origem_tipo: cartaSelecionada.tipo,
            origem_indice: cartaSelecionada.indice,
            destino_tipo: tipoDestino,
            destino_indice: indiceDestino
        };
    }

    // 5. Envio e Segurança
    try {
        socket.send(JSON.stringify(payload));
    } catch (e) {
        console.error("Erro ao enviar WebSocket:", e);
        estaProcessando = false;
    }

    // 6. Limpeza da seleção
    cartaSelecionada = null;

    // 7. MECANISMO DE RECUPERAÇÃO:
    // Independentemente de o servidor responder ou não, após 600ms
    // forçamos o servidor a reenviar o estado atual para garantir que
    // o JS e o C++ estejam sincronizados. Isso evita o travamento.
    setTimeout(() => {
        socket.send(JSON.stringify({ acao: 'OBTER_ESTADO_ATUAL' }));
        estaProcessando = false;
    }, 600);
}

function comprarCarta() {
    if (estaProcessando) return; // BLOQUEIO
    estaProcessando = true;
    tocarSom('card-slide-5.ogg');
    socket.send(JSON.stringify({ acao: 'COMPRAR_CARTA' }));
    setTimeout(() => { estaProcessando = false; }, 200); //Trava 
}

// ==========================================
// BOTÕES
// ==========================================
document.getElementById('btn-desfazer').onclick = () => {
    tocarSom('click.mp3');
    socket.send(JSON.stringify({ acao: 'DESFAZER' }));
    cartaSelecionada = null;
};

document.getElementById('btn-novo-jogo').onclick = () => {
    tocarSom('click.mp3');
    socket.send(JSON.stringify({ acao: 'NOVO_JOGO' }));
    cartaSelecionada = null;
    vitoriaJaProcessada = false;
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
    tocarSom('click.mp3');
    pararCronometro();
    document.getElementById('modal-texto').innerHTML = `
        <div class="regras-box">
            <h3>COMO JOGAR PACIÊNCIA</h3>
            <ul class="regras-lista" style="text-align: left; margin: 15px 0;">
                <li><strong>Objetivo:</strong> Mova todas as cartas para as fundações (topo) por naipe.</li>
                <li><strong>Colunas:</strong> Ordene as cartas de forma decrescente com cores alternadas.</li>
                <li><strong>Atalhos:</strong> <b>ESC</b> para pausar e <b>Ctrl+Z</b> para desfazer.</li>
                <li><strong>Arrastar:</strong> Clique nas cartas para selecionar e mover.</li>
                <li style="margin-top: 10px; color: #f0c040;"><strong>Dica:</strong> Recomendado jogar com 80% de zoom no navegador.</li>
            </ul>
        </div>
        <button onclick="fecharModal()" style="margin-top:15px; padding:10px 20px; background:#4ade80; border:none; cursor:pointer;">Entendido</button>
    `;
    document.getElementById('modal-notificacao').classList.remove('modal-oculto');
}


function mostrarPontuacao() {
    tocarSom('click.mp3');
    pararCronometro();
    
    // Você pode ajustar as regras abaixo de acordo com o sistema de pontos que seu C++ calcula
    const textoPontuacao = `
        <div class="regras-box">
            <h3>SISTEMA DE PONTUAÇÃO</h3>
            <ul class="regras-lista" style="text-align: left; margin: 15px 0;">
                <li><strong>Mover para Fundação:</strong> +10 pontos</li>
                <li><strong>Empilhar do Cava para Coluna:</strong> +15 pontos</li>
                <li><strong>Empilhar de Coluna para Coluna:</strong> +5 pontos</li>
                <li><strong>Voltar carta da Fundação:</strong> +5 pontos</li>
                <li><strong>Virar carta na Coluna:</strong> +5 pontos</li>
                <li><strong>Desfazer movimento:</strong> -15 pontos</li>
                <li><strong>Resetar Cava:</strong> -100 pontos</li>
                <li><strong>Tempo:</strong> Você ganha bônus por rapidez ao finalizar.</li>
            </ul>
        </div>
        <button onclick="fecharModal()" style="margin-top:15px; padding:10px 20px; background:#4ade80; border:none; cursor:pointer;">Entendido</button>
    `;
    
    document.getElementById('modal-texto').innerHTML = textoPontuacao;
    document.getElementById('modal-notificacao').classList.remove('modal-oculto');
}

function mostrarRecordeModal() {
    const estado = window.estadoAtual;
    const recorde = estado ? (estado.recorde || 0) : 0;
    const pontos  = estado ? (estado.pontuacao || 0) : 0;

    document.getElementById('modal-texto').innerHTML = `
        <div style="text-align: center; padding: 10px;">
            <h2 style="color:#4ade80; margin-bottom: 15px; font-family: 'Cinzel Decorative', serif;">🏅 RECORDE</h2>
            <div style="background: rgba(0,0,0,0.5); border: 1px solid rgba(255,200,50,0.2); border-radius: 8px; padding: 20px; margin-bottom: 20px; color: #fff;">
                <p style="font-size: 2em; margin: 0; color: #f0c040; font-weight: bold;">${recorde}</p>
                <p style="color: #9ca3af; margin-top: 5px; font-size: 0.9em;">pontos</p>
                <hr style="border-color: rgba(255,255,255,0.1); margin: 15px 0;">
                <p style="margin: 5px 0;">Pontuação desta partida: <strong style="color: #4ade80;">${pontos}</strong></p>
                ${pontos >= recorde ? '<p style="color:#f0c040; margin-top:10px;">⭐ Você bateu o recorde!</p>' : ''}
            </div>
            <button onclick="fecharRecordeModal()" style="padding: 10px 25px; cursor: pointer; background: rgba(240,192,64,0.2); color: #f0c040; border: 1px solid rgba(240,192,64,0.5); border-radius: 5px; font-family: 'Cinzel', serif;">FECHAR</button>
        </div>
    `;
    document.getElementById('modal-notificacao').classList.remove('modal-oculto');
}

function fecharRecordeModal() {
    // Volta para o modal de vitória se o jogo terminou, senão fecha
    if (window.estadoAtual && window.estadoAtual.vitoria) {
        // Re-exibe o modal de vitória chamando atualizarInterface com forçar re-exibição
        document.getElementById('modal-notificacao').classList.add('modal-oculto');
        // Força re-abertura do modal de vitória
        setTimeout(() => atualizarInterface(window.estadoAtual), 50);
    } else {
        document.getElementById('modal-notificacao').classList.add('modal-oculto');
    }
}


function fecharModal() {
    document.getElementById('modal-notificacao').classList.add('modal-oculto');
    if (!jogoPausado) iniciarCronometro();
}

window.jogarNovamenteModal = function() {
    socket.send(JSON.stringify({ acao: 'NOVO_JOGO' }));
    document.getElementById('modal-notificacao').classList.add('modal-oculto');
    cartaSelecionada = null;
    vitoriaJaProcessada = false;
    zerarCronometro();
    iniciarCronometro();
};

let intervaloAnimacao = null; // Mova para fora para ser acessível globalmente

function solicitarAutoCompletar() {
    if (estaProcessando) return;
    document.getElementById('container-auto').style.display = 'none';
    estaProcessando = true;

    intervaloAnimacao = setInterval(() => {
        // Se o jogo acabou, limpa e para
        if (window.estadoAtual && window.estadoAtual.vitoria) {
            pararAutoCompletar();
            return;
        }

        socket.send(JSON.stringify({ acao: 'MOVER_UMA_PARA_FUNDACAO' }));
    }, 500); 
}

function pararAutoCompletar() {
    if (intervaloAnimacao) {
        clearInterval(intervaloAnimacao);
        intervaloAnimacao = null;
    }
    estaProcessando = false;
    console.log("Auto-completar finalizado ou pausado.");
}

function processarDrop(event, tipoDestino, indiceDestino) {
    event.preventDefault();
    
    // 1. Verificação de segurança: se já estiver processando, não faz nada
    if (estaProcessando) {
        console.warn("Bloqueado: processamento de drop já está em curso.");
        return;
    }

    // 2. Bloqueia a interface
    estaProcessando = true;

    const origemRaw = event.dataTransfer.getData('app/jogo');
    
    // 3. Se por acaso não houver dados, libera a trava antes de sair
    if (!origemRaw) {
        estaProcessando = false;
        return;
    }

    const origem = JSON.parse(origemRaw);

    // 4. Validação: se o movimento for contra as regras, libera a trava e sai
    if (!ehMovimentoValido(origem, tipoDestino, indiceDestino)) {
        console.log("Drop inválido, liberando trava.");
        estaProcessando = false; 
        return;
    }

    // 5. Execução do movimento
    if (tipoDestino === 'fundacao') tocarSom('card-slide-1.ogg');
    else if (tipoDestino === 'coluna') tocarSom('card-place-3.ogg');

    let payload = {};

    // Estrutura do payload conforme sua lógica atual
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

    // 6. Envio para o servidor
    try {
        socket.send(JSON.stringify(payload));
    } catch (e) {
        console.error("Erro ao enviar WebSocket via Drag&Drop:", e);
        estaProcessando = false;
        return;
    }

    // 7. MECANISMO DE RECUPERAÇÃO:
    // Garante que a trava caia em 600ms, forçando sincronização com o estado do C++
    setTimeout(() => {
        socket.send(JSON.stringify({ acao: 'OBTER_ESTADO_ATUAL' }));
        cartaSelecionada = null;
        estaProcessando = false;
    }, 600);
}