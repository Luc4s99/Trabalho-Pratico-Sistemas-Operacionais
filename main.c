/************************************************************************************
*	Trabalho pr�tico de Sistemas Operacionais										/
*	Autores: Breno Nunes Batista, Lucas Mateus Menezes Silva						/
*																					/
************************************************************************************/		

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>//Inclus�o da biblioteca de threads

/*Tipo Abstrato de Dados*/

FILE *file;//Ponteiro de arquivo para manipula��o dos arquivos

int InfoMatriz1[2];//Cont�m o n�mero de linhas da matriz 1 na primeira posi��o e o n�mero de colunas na segunda
int InfoMatriz2[2];//Cont�m o n�mero de linhas da matriz 2 na primeira posi��o e o n�mero de colunas na segunda
float **Matriz1, **Matriz2, **MatrizResultado;//Declara��o daas matrizes usadas, todas ser�o din�micas

int VerificaArquivos(char arquivo1[], char arquivo2[]);//Verifica se � poss�vel multiplicar as matrizes
float **CriaMatrizes(char arquivo[]);//Cria as matrizes dinamicamente
void *Multiplica(void *arg);//Fun��o de execu��o das threads
void PreencheMatriz(float **Matriz, char arquivo[]);//Preenche as matrizes com os dados do arquivo do parametro
void EscreveMatriz();//Printa a matriz resultante no console
void GeraArquivo();//Gera o arquivo de saida com a atriz resultante
void Liberar();//Libera a mem�ria gasta para alocar as matrizes

/*Fun��o Principal*/

int main(int argc, char *argv[]) {
	pthread_t thread;//Declara a thread que ser� usada
	int Argumentos[2];//Declara o vetor de argumentos que ser� passado para a thread

	if(VerificaArquivos(argv[1], argv[2])){//Se as matrizes puderem ser multiplicadas
		Matriz1 = CriaMatrizes(argv[1]);//Cria��o da matriz 1
		Matriz2 = CriaMatrizes(argv[2]);//Cria��o da matriz 2

		PreencheMatriz(Matriz1, argv[1]);//Preenche a matriz 1 com os dados do arquivo disponibilizado
		PreencheMatriz(Matriz2, argv[2]);//Preenche a matriz 2 com os dados do arquivo disponibilizado

		/*Matriz resultado n�o � criada com a fun��o de cria��o pois n�o possui um arquivo com seus dados, ela � inicializada com 0*/
		MatrizResultado = (float**) malloc(InfoMatriz1[0] * sizeof(float*));
		for(int i = 0; i < InfoMatriz1[0]; i++){
			MatrizResultado[i] = (float*) malloc(InfoMatriz2[1] * sizeof(float));
			for (int j = 0; j < InfoMatriz2[1]; j++){
				MatrizResultado[i][j] = 0.0;
			}
		}

		/*Percorrendo matriz resultante*/
		for (int i = 0; i < InfoMatriz1[0]; i++){//Mesmo n�mero de linhas  da matriz 1
			for(int j = 0; j < InfoMatriz2[1]; j++){//Mesmo n�mero de colunas da matriz 2
				for(int x = 0; x < InfoMatriz2[0]; x++){//Numero de elementos que ser�o multiplicados
					Argumentos[0] = i;//Guardando o �ndice da linha do elemento a calcular
					Argumentos[1] = j;//Guardando o �ndice da coluna do elemento a calcular
					//Argumentos[2] = x;
					pthread_create(&thread, NULL, Multiplica, (void *) &Argumentos);//Cria��o da thread e passagem dos argumentos para a mesma
				}
			}
		}
		pthread_join(thread, NULL);//Espera todas as threads terminarem para encerrar
		EscreveMatriz();//Escreve a matriz no console do usu�rio
		GeraArquivo();//Gera o arquivo com os dados computados
		Liberar();//Libera toda a mem�ria usada pelas matrizes
	}else{//Se n�o for poss�vel multiplicar as matrizes
		printf("\nNao e possivel multiplicar as matrizes\n\nObs: o numero de colunas da matriz 1 deve ser igual ao de de linhas da matriz 2");
		getchar();
		Liberar();
		exit(1);
	}
	
	return 0;
}

int VerificaArquivos(char arquivo1[], char arquivo2[]){
	
	char numeroLinhas[3], numeroColunas[3], aux[3];//Vari�veis para a leitura dos dados nos arquivos
	
	file = fopen(arquivo1, "r");//Abertura do arquivo para a leitura
	if(file == NULL){//Se o ponteiro estiver nulo ��o foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo\n");
		getchar();
		Liberar();
		exit(1);
	}
	fscanf(file, "%s", aux);//Lendo dados
	InfoMatriz1[0] = atoi(aux);//Convertendo dados para inteiro
	fscanf(file, "%s", numeroColunas);//Lendo o n�mero de colunas da matriz 1
	InfoMatriz1[1] = atoi(numeroColunas);//Leitura convertida para inteiro
	fclose(file);//Fechando o arquivo usado na leitura
	
	file = fopen(arquivo2, "r");//Abrind o segundo arquivo com os dados da segunda matriz
	if(file == NULL){//Se o ponteiro estiver nulo n�o foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo\n");
		getchar();
		Liberar();
		exit(1);
	}
	fscanf(file, "%s", numeroLinhas);//Lendo o n�mero de linhas da segunda matriz
	InfoMatriz2[0] = atoi(numeroLinhas);//Convertendo leitura em inteiro
	fgetc(file);//Saltando uma linha
	fscanf(file, "%s", aux);//Lendo dados 
	InfoMatriz2[1] = atoi(aux);//COnvertendo dados para inteiro

	fclose(file);//Fechando arquivo usado na leitura
	
	if(!strcmp(numeroColunas, numeroLinhas)){//Se o n�mero de colunas da matriz 1 for igual ao n�mero de linhas da matriz 2
		return 1;//� retornado 1
	}else{
		return 0;//Sen�o � retornado 0
	}
}

float **CriaMatrizes(char arquivo[]){
	char NL[3], NC[3];//NUmero de linhas e numero de colunas
	int i, j;
	
	file = fopen(arquivo, "r");//Abertura do arquivo para leitura
	if(file == NULL){//Se o ponteiro estiver nulo n�o foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo\n");
		getchar();
		exit(1);
	}
	fscanf(file, "%s", NL);//Lendo o n�mero de linhas
	fgetc(file);
	fscanf(file, "%s", NC);//Lendo o n�mero de colunas
	fclose(file);	

	float **Matriz = (float**) malloc(atoi(NL) * sizeof(float*));//Criando a matriz dinamicamente
	for(i = 0; i < atoi(NL); i++){
		Matriz[i] = (float*) malloc(atoi(NC) * sizeof(float));
		for (j = 0; j < atoi(NC); j++){
			Matriz[i][j] = 0.0;//Inicialmente preenchida com zeros
		}
	}
	return Matriz;//Retornando a matriz j� criada
}

void PreencheMatriz(float **Matriz, char arquivo[]){
	float elemento;//Vari�vel para cada elemento da matriz
	int linha, coluna;

	file = fopen(arquivo, "r");//Abertura do arquivo para a leitura

	if(file == NULL){//Se o ponteiro estiver nulo n�o foi possivel abrir o arquivo
		printf("Erro ao abrir o arquivo");
		getchar();
		Liberar();
		exit(1);
	}

	fscanf(file, "%d", &linha);//Lendo o n�mero de linhas
	fgetc(file);
	fscanf(file, "%d", &coluna);//Lendo o n�mero de colunas
	fgetc(file);
	
	for(int i = 0; i < linha; i++){//For com o n�mero de linhas lido
		for(int j = 0; j < coluna; j++){//For com o n�mero de colunas lido
			fscanf(file, "%f", &Matriz[i][j]);//Lendo o elemento do arquivo e inserindo-o na matriz
			fgetc(file);//Saltando o espa�o
		}
	}
}

void *Multiplica(void *arg){//Fun��o que a thread executa
	int *Argumentos = (int*) arg;//Declarando os argumentos
	int i;
	float soma = 0.0;//Soma que ser� colocada na matriz resultante
	
	for (i = 0; i < InfoMatriz2[0]; i++){//For com a quantidade de opera��es a serem realizadas
		soma = soma + (Matriz1[Argumentos[0]][i] * Matriz2[i][Argumentos[1]]);//Soma sendo incrementada
	}
	
	MatrizResultado[Argumentos[0]][Argumentos[1]] = soma;//Resultado da soma � colocado na matriz resultante em sua respectiva posi��o
}

void EscreveMatriz(){
	int i, j;
	printf(" %d\n %d\n", InfoMatriz1[0], InfoMatriz2[1]);//Printa o n�mero de linhas e colunas da matriz

	for(i = 0; i < InfoMatriz1[0]; i++){//For com o n�mero de linhas da matrz 1
		for(j = 0; j < InfoMatriz2[1]; j++){//For com o n�mero de colunas da matriz 2
			printf(" %.2f ", MatrizResultado[i][j]);//Printando o elemento da matriz na respectiva posi��o
		}
		printf("\n");//Saltando ma linha
	}
}

void GeraArquivo(){
	file = fopen("resultado.txt", "w");//Criando o arquivode sa�da

	if(file == NULL){//Se o ponteiro estiver nulo n�o foi possivel abrir o arquivo
		printf("Erro ao criar o arquivo de saida");
		getchar();
		Liberar();
		exit(1);
	}

	fprintf(file, " %d\n", InfoMatriz1[0]);//Ecrevendo o n�mero de linhas
	fprintf(file, " %d\n", InfoMatriz2[1]);//Ecrevendo o n�mero de colunas

	for(int i = 0; i < InfoMatriz1[0]; i++){//For com o n�mero de linhas da matriz 1
		for(int j = 0; j < InfoMatriz2[1]; j++){//FOr com o n�mero de colunas da matriz 2
			fprintf(file, " %.2f ", MatrizResultado[i][j]);//Escrevendo o elemento no arquivo
		}
		fprintf(file, "\n");//Escrevendo uma quebra de linha
	}
	fclose(file);//Fechando o arquivo e salvando
}

void Liberar(){
	free(Matriz1);//Libera a mem�ria usada na matriz 1
	free(Matriz2);//Libera a mem�ria usada na matriz 2
	free(MatrizResultado);//Libera a mem�ria usada na matriz resultado
}