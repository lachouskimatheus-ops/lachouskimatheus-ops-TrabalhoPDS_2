let socket=null,estadoAtual=null,selecionadas=[],tentativaReconexao=null;
const $=id=>document.getElementById(id);
const sala=new URLSearchParams(location.search).get("sala")||"";
const telaConexao=$("tela-conexao"),telaJogo=$("tela-jogo"),nomeInput=$("nome-jogador"),btnConectar=$("btn-conectar");
const mensagemConexao=$("mensagem-conexao"),maoLocal=$("mao-local"),adversarios=$("adversarios");
const btnConfirmar=$("btn-confirmar"),btnNovaRodada=$("btn-nova-rodada"),slots=[...document.querySelectorAll("#slots-troca span")];

inicializar();

function inicializar(){
    $("sala-conexao").textContent=sala?`Sala: ${sala}`:"Sala não informada";
    $("sala-jogo").textContent=sala||"---";
    nomeInput.value=localStorage.getItem("poker_nome")||"";
    btnConectar.onclick=conectar;
    btnConfirmar.onclick=confirmarTroca;
    btnNovaRodada.onclick=()=>enviar({tipo:"acao_jogo",acao:"NOVA_RODADA"});
    nomeInput.onkeydown=e=>{if(e.key==="Enter")conectar()};
    if(!sala){mensagemConexao.textContent="O link não possui o código da sala.";btnConectar.disabled=true}
}

function conectar(){
    const nome=nomeInput.value.trim();
    if(!sala)return;
    if(nome.length<2){mensagemConexao.textContent="Digite um nome com pelo menos 2 caracteres.";return nomeInput.focus()}
    localStorage.setItem("poker_nome",nome);
    mensagemConexao.textContent="Conectando...";
    btnConectar.disabled=true;
    btnConectar.textContent="Conectando...";
    abrirWebSocket();
}

function abrirWebSocket(){
    if(socket&&socket.readyState<2)socket.close();
    const protocolo=location.protocol==="https:"?"wss":"ws";
    socket=new WebSocket(`${protocolo}://${location.host}/ws/poker`);

    socket.onopen=()=>atualizarConexao("conectando","Conectando à sala");
    socket.onmessage=e=>{
        let msg;
        try{msg=JSON.parse(e.data)}catch{return mostrarErro("Mensagem inválida recebida do servidor.")}
        processarMensagem(msg);
    };
    socket.onerror=()=>atualizarConexao("desconectado","Erro de conexão");
    socket.onclose=()=>{
        atualizarConexao("desconectado","Conexão perdida");
        if(!telaJogo.classList.contains("escondido")){
            $("mensagem-principal").textContent="Conexão perdida";
            $("mensagem-secundaria").textContent="Tentando reconectar...";
            clearTimeout(tentativaReconexao);
            tentativaReconexao=setTimeout(abrirWebSocket,1500);
        }else{
            btnConectar.disabled=false;
            btnConectar.textContent="Conectar à mesa";
        }
    };
}

function processarMensagem(msg){
    if(msg.tipo==="conectado")return enviarEntrada();
    if(msg.tipo==="entrada_confirmada"||msg.tipo==="reconexao_confirmada"){
        telaConexao.classList.add("escondido");
        telaJogo.classList.remove("escondido");
        atualizarConexao("conectado","Conectado à sala");
        return;
    }
    if(msg.tipo==="estado_jogo"){
        estadoAtual=msg;
        selecionadas=[];
        atualizarTela(msg);
        return;
    }
    if(msg.tipo==="erro")mostrarErro(msg.mensagem||msg.erro||"Erro desconhecido.");
}

function enviarEntrada(){
    enviar({
        tipo:"entrar_sala",
        sala,
        nome:localStorage.getItem("poker_nome")||nomeInput.value.trim(),
        token:obterToken()
    });
}

function obterToken(){
    const chave=`poker_token_${sala}`;
    let token=localStorage.getItem(chave);
    if(!token){
        token=crypto.randomUUID?crypto.randomUUID():`poker-${Date.now()}-${Math.random().toString(36).slice(2)}`;
        localStorage.setItem(chave,token);
    }
    return token;
}

function atualizarTela(e){
    const jogadores=Array.isArray(e.jogadores)?e.jogadores:[];
    const local=jogadores.find(j=>j.id===e.meu_id);
    const conectados=e.jogadores_conectados??0,maximo=e.max_jogadores??jogadores.length;

    $("rodada").textContent=e.rodada??0;
    $("empates").textContent=e.empates??0;
    $("modo").textContent=e.modo==="COMPUTADOR"?"Computador":"Multiplayer";
    $("quantidade-jogadores").textContent=`${conectados}/${maximo}`;
    $("jogadores-conexao").textContent=`Jogadores: ${conectados}/${maximo}`;
    $("fase-conexao").textContent=nomeFase(e.fase);
    $("descricao-partida").textContent=e.modo==="COMPUTADOR"?"Partida contra o computador.":`Mesa para ${maximo} jogadores.`;
    $("mensagem-principal").textContent=e.mensagem||nomeFase(e.fase);
    $("mensagem-secundaria").textContent=descricaoFase(e);

    atualizarLocal(local,e);
    atualizarAdversarios(jogadores.filter(j=>j.id!==e.meu_id));
    atualizarControles(e);
    atualizarSlots();
}

function atualizarLocal(jogador,e){
    $("nome-local").textContent=jogador?.nome||localStorage.getItem("poker_nome")||"Jogador";
    $("pontos-local").textContent=jogador?.pontos??0;
    $("jogada-local").textContent=jogador?.jogada&&jogador.jogada!=="Oculta"?jogador.jogada:"Jogada ainda não revelada";
    $("status-confirmacao").textContent=jogador?.confirmou_troca?"Troca confirmada":"Troca não confirmada";
    const mao=Array.isArray(e.minha_mao)&&e.minha_mao.length?e.minha_mao:(jogador?.mao||[]);
    desenharMao(maoLocal,mao,e.pode_confirmar_troca===true);
}

function atualizarAdversarios(lista){
    adversarios.innerHTML="";
    lista.forEach(j=>{
        const area=document.createElement("article");
        area.className=`adversario${j.conectado?"":" desconectado"}`;
        area.innerHTML=`
            <div class="adversario-cabecalho">
                <div><h3>${escapar(j.nome||`Jogador ${j.id+1}`)}</h3><p>${estadoJogador(j)}</p></div>
                <div class="pontos"><span>Pontos</span><strong>${j.pontos??0}</strong></div>
            </div>
            <div class="mao mao-adversario"></div>
            <p>${j.jogada&&j.jogada!=="Oculta"?`Jogada: ${escapar(j.jogada)}`:`${j.quantidade_ultima_troca??0} carta(s) trocada(s)`}</p>`;
        desenharMao(area.querySelector(".mao-adversario"),Array.isArray(j.mao)?j.mao:[],false);
        adversarios.appendChild(area);
    });
}

function desenharMao(container,cartas,selecionavel){
    container.innerHTML="";
    cartas.forEach((carta,i)=>{
        const el=document.createElement("div");
        el.className="carta";
        if(!carta||carta.oculta||carta.valor==null||carta.naipe==null)el.classList.add("oculta");
        else el.style.backgroundImage=`url("${caminhoCarta(carta)}")`;

        if(selecionavel&&!el.classList.contains("oculta")){
            el.classList.add("selecionavel");
            if(selecionadas.includes(i))el.classList.add("selecionada");
            el.onclick=()=>alternarCarta(i);
        }
        container.appendChild(el);
    });
}

function caminhoCarta(carta){
    const naipes=["clubs","hearts","spades","diamonds"];
    const valores=["","ace","02","03","04","05","06","07","08","09","10","jack","queen","king"];
    return `/assets/cartas/${naipes[Number(carta.naipe)]}_${valores[Number(carta.valor)]}.png`;
}

function alternarCarta(indice){
    if(!estadoAtual?.pode_confirmar_troca)return;
    const pos=selecionadas.indexOf(indice);
    if(pos>=0)selecionadas.splice(pos,1);
    else{
        if(selecionadas.length>=3)return $("mensagem-secundaria").textContent="Você pode selecionar no máximo três cartas.";
        selecionadas.push(indice);
    }
    const local=estadoAtual.jogadores?.find(j=>j.id===estadoAtual.meu_id);
    desenharMao(maoLocal,estadoAtual.minha_mao?.length?estadoAtual.minha_mao:(local?.mao||[]),true);
    atualizarSlots();
    atualizarControles(estadoAtual);
}

function atualizarControles(e){
    btnConfirmar.disabled=!e.pode_confirmar_troca;
    btnConfirmar.textContent=selecionadas.length?`Confirmar troca (${selecionadas.length})`:"Manter todas as cartas";
    btnNovaRodada.classList.toggle("escondido",!e.pode_iniciar_nova_rodada);
    btnNovaRodada.disabled=!e.pode_iniciar_nova_rodada;
}

function atualizarSlots(){
    slots.forEach((slot,i)=>slot.classList.toggle("ativo",i<selecionadas.length));
    $("contador-selecionadas").textContent=`Selecionadas: ${selecionadas.length}/3`;
}

function confirmarTroca(){
    if(!estadoAtual?.pode_confirmar_troca)return;
    enviar({tipo:"acao_jogo",acao:"CONFIRMAR_TROCA",indices:[...selecionadas]});
    btnConfirmar.disabled=true;
    $("status-confirmacao").textContent="Enviando confirmação...";
}

function enviar(obj){
    if(socket?.readyState===WebSocket.OPEN)socket.send(JSON.stringify(obj));
    else mostrarErro("A conexão com o servidor não está aberta.");
}

function atualizarConexao(classe,texto){
    const ponto=$("ponto-conexao");
    ponto.className=`ponto ${classe}`;
    $("texto-conexao").textContent=texto;
}

function mostrarErro(texto){
    if(!telaConexao.classList.contains("escondido")){
        mensagemConexao.textContent=texto;
        btnConectar.disabled=false;
        btnConectar.textContent="Conectar à mesa";
    }else{
        $("mensagem-principal").textContent="Não foi possível realizar a ação";
        $("mensagem-secundaria").textContent=texto;
    }
}

function nomeFase(fase){
    return({
        AGUARDANDO_JOGADORES:"Aguardando jogadores",
        ESCOLHENDO_TROCAS:"Escolhendo trocas",
        AGUARDANDO_OUTROS_JOGADORES:"Aguardando jogadores",
        RESULTADO:"Resultado da rodada"
    })[fase]||"Aguardando estado";
}

function descricaoFase(e){
    if(e.fase==="AGUARDANDO_JOGADORES")return"A partida começará quando todos entrarem.";
    if(e.fase==="ESCOLHENDO_TROCAS")return e.pode_confirmar_troca?"Selecione até três cartas e confirme.":"Aguarde sua ação ser liberada.";
    if(e.fase==="AGUARDANDO_OUTROS_JOGADORES")return"Sua troca foi registrada. Aguarde os demais.";
    if(e.fase==="RESULTADO"){
        if(e.vencedor===-1)return"A rodada terminou empatada.";
        return e.vencedor===e.meu_id?"Você venceu esta rodada.":"Outro jogador venceu esta rodada.";
    }
    return e.mensagem||"";
}

function estadoJogador(j){
    if(j.computador)return"Computador";
    if(!j.conectado)return"Desconectado";
    return j.confirmou_troca?"Troca confirmada":"Escolhendo cartas";
}

function escapar(texto){
    const el=document.createElement("div");
    el.textContent=texto;
    return el.innerHTML;
}