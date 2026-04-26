#pragma once
#include <string>
#include <vector>

class Carta; //Declaração antecipada que evita uma porrada de problema de compilação

class Jogador {
protected:
	//O protected é tipo um private que as classes filhas podem acessar
	int id_; //Aqui eu vou começar a usar essa convenção que o próprio Thiago usa de colocar _ em atributos
	std::string nome_;
	std::vector<Carta*> mao_; //Usando um vetor de ponteiros, a gente garante que cada carta vai ser única e vai ser muito mais rápido o processamento

public:
	Jogador(int id, std::string nome);
	void receberCarta(Carta* novaCarta);
	virtual ~Jogador(); //Esse virtual basicamente garante que a interação entre as classes mãe e filha não vai ignorar nenhum atributo ou método
	void mostrarMao() const;
	virtual void limparMao();

	//Getters:
	std::string getNome() const;
	int getId() const;

	//Esses métodos ainda podem mudar ou podem ter outros métodos aqui, à medida que a gente for programando os outros jogos

};