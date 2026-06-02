let cartaSelecionada = null;

const cartas = document.querySelectorAll("#minha-mao .carta");
const mesaDescarte = document.getElementById("mesa-descarte");
const qtdCartas = document.getElementById("qtd-cartas");

cartas.forEach((carta) => {
    carta.addEventListener("click", () => {
        if (cartaSelecionada) {
            cartaSelecionada.classList.remove("selecionada");
        }

        cartaSelecionada = carta;
        cartaSelecionada.classList.add("selecionada");
    });
});

document.getElementById("btn-comprar-monte").addEventListener("click", () => {

    const novaCarta = document.createElement("div");

    novaCarta.classList.add("carta");

    novaCarta.textContent = "9♦";

    novaCarta.addEventListener("click", () => {

        if (cartaSelecionada) {
            cartaSelecionada.classList.remove("selecionada");
        }

        cartaSelecionada = novaCarta;
        cartaSelecionada.classList.add("selecionada");
    });

    document.getElementById("minha-mao").appendChild(novaCarta);

    atualizarQuantidadeCartas();
});

document.getElementById("btn-comprar-mesa").addEventListener("click", () => {

    const cartaMesa = mesaDescarte.querySelector(".carta");

    if (!cartaMesa || cartaMesa.classList.contains("carta-vazia")) {
        mostrarModal("Não há carta na mesa.");
        return;
    }

    document.getElementById("minha-mao").appendChild(cartaMesa);

    atualizarQuantidadeCartas();
});

document.getElementById("btn-bater").addEventListener("click", () => {
    mostrarModal("Você tentou bater.");
});

document.getElementById("btn-organizar").addEventListener("click", () => {
    mostrarModal("Mão organizada.");
});

mesaDescarte.addEventListener("click", () => {
    if (!cartaSelecionada) {
        mostrarModal("Selecione uma carta antes de descartar.");
        return;
    }

    mesaDescarte.innerHTML = "";
    mesaDescarte.appendChild(cartaSelecionada);

    cartaSelecionada.classList.remove("selecionada");
    cartaSelecionada = null;

    atualizarQuantidadeCartas();
});

function atualizarQuantidadeCartas() {
    const total = document.querySelectorAll("#minha-mao .carta").length;
    qtdCartas.textContent = total;
}

function mostrarModal(texto) {
    const modal = document.getElementById("modal-notificacao");
    const modalTexto = document.getElementById("modal-texto");

    modalTexto.textContent = texto;
    modal.classList.remove("modal-oculto");

    setTimeout(() => {
        modal.classList.add("modal-oculto");
    }, 1200);
}