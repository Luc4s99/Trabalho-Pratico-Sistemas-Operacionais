/************************************************************************************
*	Trabalho prático de Sistemas Operacionais										/
*	Autores: Breno Nunes Batista, Lucas Mateus Menezes Silva						/
*																					/
************************************************************************************/		

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>//Inclusão da biblioteca de threads

/*Tipo Abstrato de Dados*/

FILE *file;//Ponteiro de arquivo para manipulação dos arquivos

int InfoMatriz1[2];//Contém o número de linhas da matriz 1 na primeira posição e o número de colunas na segunda
int InfoMatriz2[2];//Contém o número de linhas da matriz 2 na primeira posição e o número de colunas na segunda
float **Matriz1, **Matriz2, **MatrizResultado;//Declaração daas matrizes usadas, todas serão dinâmicas

int VerificaArquivos(char arquivo1[], char arquivo2[]);//Verifica se é possível multiplicar as matrizes
float **CriaMatrizes(char arquivo[]);//Cria as matrizes dinamicamente
void *Multiplica(void *arg);//Função de execução das threads
void PreencheMatriz(float **Matriz, char arquivo[]);//Preenche as matrizes com os dados do arquivo do parametro
void EscreveMatriz();//Printa a matriz resultante no console
void GeraArquivo();//Gera o arquivo de saida com a atriz resultante
void Liberar();//Libera a memória gasta para alocar as matrizes

/*Função Principal*/

int main(int argc, char *argv[]) {
	pthread_t thread;//Declara a thread que será usada
	int Argumentos[2];//Declara o vetor de argumentos que será passado para a thread

	if(VerificaArquivos(argv[1], argv[2])){//Se as matrizes puderem ser multiplicadas
		Matriz1 = CriaMatrizes(argv[1]);//Criação da matriz 1
		Matriz2 = CriaMatrizes(argv[2]);//Criação da matriz 2

		PreencheMatriz(Matriz1, argv[1]);//Preenche a matriz 1 com os dados do arquivo disponibilizado
		PreencheMatriz(Matriz2, argv[2]);//Preenche a matriz 2 com os dados do arquivo disponibilizado

		/*Matriz resultado não é criada com a função de criação pois não possui um arquivo com seus dados, ela é inicializada com 0*/
		MatrizResultado = (float**) malloc(InfoMatriz1[0] * sizeof(float*));
		for(int i = 0; i < InfoMatriz1[0]; i++){
			MatrizResultado[i] = (float*) malloc(InfoMatriz2[1] * sizeof(float));
			for (int j = 0; j < InfoMatriz2[1]; j++){
				MatrizResultado[i][j] = 0.0;
			}
		}

		/*Percorrendo matriz resultante*/
		for (int i = 0; i < InfoMatriz1[0]; i++){//Mesmo número de linhas  da matriz 1
			for(int j = 0; j < InfoMatriz2[1]; j++){//Mesmo número de colunas da matriz 2
				for(int x = 0; x < InfoMatriz2[0]; x++){//Numero de elementos que serão multiplicados
					Argumentos[0] = i;//Guardando o índice da linha do elemento a calcular
					Argumentos[1] = j;//Guardando o índice da coluna do elemento a calcular
					//Argumentos[2] = x;
					pthread_create(&thread, NULL, Multiplica, (void *) &Argumentos);//Criação da thread e passagem dos argumentos para a mesma
				}
			}
		}
		pthread_join(thread, NULL);//Espera todas as threads terminarem para encerrar
		EscreveMatriz();//Escreve a matriz no console do usuário
		GeraArquivo();//Gera o arquivo com os dados computados
		Liberar();//Libera toda a memória usada pelas matrizes
	}else{//Se não for possível multiplicar as matrizes
		printf("\nNao e possivel multiplicar as matrizes\n\nObs: o numero de colunas da matriz 1 deve ser igual ao de de linhas da matriz 2");
		getchar();
		Liberar();
		exit(1);
	}
	
	return 0;
}

int VerificaArquivos(char arquivo1[], char arquivo2[]){
	
	char numeroLinhas[3], numeroColunas[3], aux[3];//Variáveis para a leitura dos dados nos arquivos
	
	file = fopen(arquivo1, "r");//Abertura do arquivo para a leitura
	if(file == NULL){//Se o ponteiro estiver nulo ñão foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo\n");
		getchar();
		Liberar();
		exit(1);
	}
	fscanf(file, "%s", aux);//Lendo dados
	InfoMatriz1[0] = atoi(aux);//Convertendo dados para inteiro
	fscanf(file, "%s", numeroColunas);//Lendo o número de colunas da matriz 1
	InfoMatriz1[1] = atoi(numeroColunas);//Leitura convertida para inteiro
	fclose(file);//Fechando o arquivo usado na leitura
	
	file = fopen(arquivo2, "r");//Abrind o segundo arquivo com os dados da segunda matriz
	if(file == NULL){//Se o ponteiro estiver nulo não foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo\n");
		getchar();
		Liberar();
		exit(1);
	}
	fscanf(file, "%s", numeroLinhas);//Lendo o número de linhas da segunda matriz
	InfoMatriz2[0] = atoi(numeroLinhas);//Convertendo leitura em inteiro
	fgetc(file);//Saltando uma linha
	fscanf(file, "%s", aux);//Lendo dados 
	InfoMatriz2[1] = atoi(aux);//COnvertendo dados para inteiro

	fclose(file);//Fechando arquivo usado na leitura
	
	if(!strcmp(numeroColunas, numeroLinhas)){//Se o número de colunas da matriz 1 for igual ao número de linhas da matriz 2
		return 1;//É retornado 1
	}else{
		return 0;//Senão é retornado 0
	}
}

float **CriaMatrizes(char arquivo[]){
	char NL[3], NC[3];//NUmero de linhas e numero de colunas
	int i, j;
	
	file = fopen(arquivo, "r");//Abertura do arquivo para leitura
	if(file == NULL){//Se o ponteiro estiver nulo não foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo\n");
		getchar();
		exit(1);
	}
	fscanf(file, "%s", NL);//Lendo o número de linhas
	fgetc(file);
	fscanf(file, "%s", NC);//Lendo o número de colunas
	fclose(file);	

	float **Matriz = (float**) malloc(atoi(NL) * sizeof(float*));//Criando a matriz dinamicamente
	for(i = 0; i < atoi(NL); i++){
		Matriz[i] = (float*) malloc(atoi(NC) * sizeof(float));
		for (j = 0; j < atoi(NC); j++){
			Matriz[i][j] = 0.0;//Inicialmente preenchida com zeros
		}
	}
	return Matriz;//Retornando a matriz já criada
}

void PreencheMatriz(float **Matriz, char arquivo[]){
	float elemento;//Variável para cada elemento da matriz
	int linha, coluna;

	file = fopen(arquivo, "r");//Abertura do arquivo para a leitura

	if(file == NULL){//Se o ponteiro estiver nulo não foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo");
		getchar();
		Liberar();
		exit(1);
	}

	fscanf(file, "%d", &linha);//Lendo o número de linhas
	fgetc(file);
	fscanf(file, "%d", &coluna);//Lendo o número de colunas
	fgetc(file);
	
	for(int i = 0; i < linha; i++){//For com o número de linhas lido
		for(int j = 0; j < coluna; j++){//For com o número de colunas lido
			fscanf(file, "%f", &Matriz[i][j]);//Lendo o elemento do arquivo e inserindo-o na matriz
			fgetc(file);//Saltando o espaço
		}
	}
}

void *Multiplica(void *arg){//Função que a thread executa
	int *Argumentos = (int*) arg;//Declarando os argumentos
	int i;
	float soma = 0.0;//Soma que será colocada na matriz resultante
	
	for (i = 0; i < InfoMatriz2[0]; i++){//For com a quantidade de operações a serem realizadas
		soma = soma + (Matriz1[Argumentos[0]][i] * Matriz2[i][Argumentos[1]]);//Soma sendo incrementada
	}
	
	MatrizResultado[Argumentos[0]][Argumentos[1]] = soma;//Resultado da soma é colocado na matriz resultante em sua respectiva posição
}

void EscreveMatriz(){
	int i, j;
	printf(" %d\n %d\n", InfoMatriz1[0], InfoMatriz2[1]);//Printa o número de linhas e colunas da matriz

	for(i = 0; i < InfoMatriz1[0]; i++){//For com o número de linhas da matrz 1
		for(j = 0; j < InfoMatriz2[1]; j++){//For com o número de colunas da matriz 2
			printf(" %.2f ", MatrizResultado[i][j]);//Printando o elemento da matriz na respectiva posição
		}
		printf("\n");//Saltando ma linha
	}
}

void GeraArquivo(){
	file = fopen("resultado.txt", "w");//Criando o arquivode saída

	if(file == NULL){//Se o ponteiro estiver nulo não foi possivel abrir o arquivo
		printf("Erro ao criar o arquivo de saida");
		getchar();
		Liberar();
		exit(1);
	}

	fprintf(file, " %d\n", InfoMatriz1[0]);//Ecrevendo o número de linhas
	fprintf(file, " %d\n", InfoMatriz2[1]);//Ecrevendo o número de colunas

	for(int i = 0; i < InfoMatriz1[0]; i++){//For com o número de linhas da matriz 1
		for(int j = 0; j < InfoMatriz2[1]; j++){//FOr com o número de colunas da matriz 2
			fprintf(file, " %.2f ", MatrizResultado[i][j]);//Escrevendo o elemento no arquivo
		}
		fprintf(file, "\n");//Escrevendo uma quebra de linha
	}
	fclose(file);//Fechando o arquivo e salvando
}

void Liberar(){
	free(Matriz1);//Libera a memória usada na matriz 1
	free(Matriz2);//Libera a memória usada na matriz 2
	free(MatrizResultado);//Libera a memória usada na matriz resultado
}