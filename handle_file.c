#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/handle_file.h"

// Constantes de pré-processamento
#define IN_DISK_REG_SIZE 87
#define IN_DISK_HEADER_SIZE 5

typedef struct t_header
{
	int topoPilha;
	char status;
} HEADER;

void file_read_csv_write_binary(const char *nome_arq_dados, const char *nome_arq_binario)
{
	if(nome_arq_dados != NULL)
	{
		int codigoINEP = 0, escola_size = 0, cidade_size = 0, prestadora_size = 0, total_bytes = 0;
		char byte_padding = '0', prestadora[10], data[11], escola[50], cidade[70], uf[3], line[300], *token = NULL;
		HEADER binario_h;
		FILE *csv = NULL, *binario = NULL;
		binario_h.topoPilha = -1;
		binario_h.status = '0';
		csv = fopen(nome_arq_dados, "r");
		binario = fopen(nome_arq_binario, "wb");
		fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
		while(1)
		{
			strcpy(uf, "00");
			strcpy(data, "0000000000");
			strcpy(escola, "");
			strcpy(cidade, "");
			strcpy(prestadora, "");
			fgets(line, sizeof(line), csv);
			token = line;
			if(feof(csv) == 0) // Se o fim do arquivo nao foi setado
			{
				if(token[0] != ';')
				{
					sscanf(token, " %10[^;]", prestadora);
					prestadora_size = strlen(prestadora);
					token = strchr(token, ';');
				}
				else
				{
					prestadora_size = 0;
				}
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %10[^;]", data);
					token = strchr(token, ';');
				}
				++token;
				sscanf(token, "%d", &codigoINEP);
				token = strchr(token, ';');
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %50[^;\n]", escola);
					escola_size = strlen(escola);
					token = strchr(token, ';');
				}
				else
				{
					escola_size = 0;
				}
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %70[^;\n]", cidade);
					cidade_size = strlen(cidade);
					token = strchr(token, ';');
				}
				else
				{
					cidade_size = 0;
				}
				++token;
				if(token[0] != ';')
				{
					sscanf(token, " %s", uf);
				}
				fwrite(&codigoINEP, sizeof(codigoINEP), 1, binario);
				fwrite(data, strlen(data), 1, binario);
				fwrite(uf, strlen(uf), 1, binario);
				fwrite(&escola_size, sizeof(int), 1, binario);
				if(escola_size > 0)
				{
					fwrite(escola, escola_size, 1, binario);
				}
				fwrite(&cidade_size, sizeof(int), 1, binario);
				if(cidade_size > 0)
				{
					fwrite(cidade, cidade_size, 1, binario);
				}
				fwrite(&prestadora_size, sizeof(int), 1, binario);
				if(prestadora_size > 0)
				{
					fwrite(prestadora, prestadora_size, 1, binario);
				}
				total_bytes = 28 + escola_size + prestadora_size + cidade_size;
				if(total_bytes < IN_DISK_REG_SIZE)
				{
					fwrite(&byte_padding, sizeof(char), (IN_DISK_REG_SIZE - total_bytes), binario);
				}
			}
			else
			{
				break;
			}
		}
		rewind(binario);
		binario_h.status = '1';
		fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		fclose(binario);
		fclose(csv);
		printf("Arquivo carregado.\n");
	}
	else
	{
		printf("Falha no carregamento do arquivo.\n");
	}
}

void file_read_all_binary(const char *nome_arq_binario){
	if(nome_arq_binario != NULL){
		FILE *binario = fopen(nome_arq_binario, "r+b");
		HEADER binario_h;
		binario_h.status = '0';
		int campos_variaveis_size = 0, codigoINEP, reg_size = 28;
		char prestadora[10], data[11], escola[50], cidade[70], uf[3];

		if(binario == NULL) {
			printf("Arquivo inexistente.");
		}

		fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		fseek(binario, (IN_DISK_HEADER_SIZE - 1), SEEK_CUR);

		while(1) {
			memset(uf, 0x00, sizeof(uf));
			memset(escola, 0x00, sizeof(escola));
			memset(cidade, 0x00, sizeof(cidade));
			memset(prestadora, 0x00, sizeof(prestadora));
			fread(&codigoINEP, sizeof(codigoINEP), 1, binario);
			if(codigoINEP != -1){
				printf("%d ", codigoINEP);
				fread(data, (sizeof(data) - 1), 1, binario);
				if(data[0] != '0')
				{
					printf("%s ", data);
				}
				fread(uf, (sizeof(uf) - 1), 1, binario);
				if(uf[0] != '0')
				{
					printf("%s ", uf);
				}
				fread(&campos_variaveis_size, sizeof(int), 1, binario);
				reg_size = reg_size + campos_variaveis_size;
				fread(escola, campos_variaveis_size, 1, binario);
				if(campos_variaveis_size > 0)
				{
					printf("%d %s ", campos_variaveis_size, escola);
				}
				fread(&campos_variaveis_size, sizeof(int), 1, binario);
				reg_size = reg_size + campos_variaveis_size;
				fread(cidade, campos_variaveis_size, 1, binario);
				if(campos_variaveis_size > 0)
				{
					printf("%d %s ", campos_variaveis_size, cidade);
				}
				fread(&campos_variaveis_size, sizeof(int), 1, binario);
				reg_size = reg_size + campos_variaveis_size;
				fread(prestadora, campos_variaveis_size, 1, binario);
				if(campos_variaveis_size > 0)
				{
					printf("%d %s", campos_variaveis_size, prestadora);
				}
				printf("\n");
			}
			else
			{
				fseek(binario, IN_DISK_REG_SIZE - sizeof(int), SEEK_CUR);
			}
			if(feof(binario) == 0)
			{
				if (reg_size < IN_DISK_REG_SIZE) {
					fseek(binario, IN_DISK_REG_SIZE - reg_size, SEEK_CUR);
				}
			}
			else
			{
				break;
			}
			reg_size = 28;
		}
		rewind(binario);
		binario_h.status = '1';
		fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
		fclose(binario);
	}
	else {
		printf("Falha no processamento do arquivo.\n");
	}
}

void file_read_binary_rrn(const char *nome_arq_binario, const int rrn)
{
	if(nome_arq_binario != NULL)
	{
		FILE *binario = NULL;
		HEADER binario_h;
		binario_h.status = '0';
		int campos_variaveis_size = 0, codigoINEP = 0;
		char prestadora[10], data[11], escola[50], cidade[70], uf[3];
		memset(prestadora, 0x00, sizeof(prestadora));
		memset(escola, 0x00, sizeof(escola));
		memset(cidade, 0x00, sizeof(cidade));
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fseek(binario, (IN_DISK_REG_SIZE * (rrn - 1)) + (IN_DISK_HEADER_SIZE - 1), SEEK_CUR);
			if(fread(&codigoINEP, sizeof(codigoINEP), 1, binario) > 0)
			{
				if(codigoINEP != -1)
				{
					printf("%d ", codigoINEP);
					fread(data, (sizeof(data) - 1), 1, binario);
					if(strcmp(data, "0000000000") != 0)
					{
						printf("%s ", data);
					}
					fread(uf, (sizeof(uf) - 1), 1, binario);
					if(strcmp(uf, "00") != 0)
					{
						printf("%s ", uf);
					}
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					printf("%d ", campos_variaveis_size);
					if(campos_variaveis_size > 0)
					{
						fread(escola, campos_variaveis_size, 1, binario);
						printf("%s ", escola);
					}
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					printf("%d ", campos_variaveis_size);
					if(campos_variaveis_size > 0)
					{
						fread(cidade, campos_variaveis_size, 1, binario);
						printf("%s ", cidade);
					}
					fread(&campos_variaveis_size, sizeof(int), 1, binario);
					printf("%d ", campos_variaveis_size);
					if(campos_variaveis_size > 0)
					{
						fread(prestadora, campos_variaveis_size, 1, binario);
						printf("%s", prestadora);
					}
					printf("\n");
				}
				else
				{
					printf("Registro inexistente.\n");
				}
			}
			else
			{
				printf("Registro inexistente.\n");
			}
			rewind(binario);
			binario_h.status = '1';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fclose(binario);
		}
		else
		{
			printf("Falha no processamento do arquivo.\n");
		}
	}
	else
	{
		printf("Falha no processamento do arquivo.\n");
	}
}

void file_filter_by_criteria(const char *nome_arq_binario, const char *campo, const char *chave) {
	if (nome_arq_binario == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	FILE *binario = fopen(nome_arq_binario, "r+b");

	if(binario == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	HEADER binario_h;
	binario_h.status = '0';
	int campos_variaveis_size = 0, codigoINEP = 0, printRegister = 0;
	int escolaChecker = 0, prestadoraChecker = 0, cidadeChecker = 0, reg_size = 28;
	char prestadora[10], data[11], escola[50], cidade[70], uf[3];
	memset(prestadora, 0x00, sizeof(prestadora));
	memset(escola, 0x00, sizeof(escola));
	memset(cidade, 0x00, sizeof(cidade));
	binario = fopen(nome_arq_binario, "r+b");

	fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
	fseek(binario, (IN_DISK_HEADER_SIZE - 1), SEEK_CUR);

	while(1) {
		memset(uf, 0x00, sizeof(uf));
		memset(escola, 0x00, sizeof(escola));
		memset(cidade, 0x00, sizeof(cidade));
		memset(prestadora, 0x00, sizeof(prestadora));

		fread(&codigoINEP, sizeof(codigoINEP), 1, binario);

		if(codigoINEP != -1){
			if(strncmp("codINEP", campo, sizeof(campo)) == 0 && codigoINEP == atoi(chave)) {
				printRegister = 1;
			}

			// CAMPOS FIXOS
			fread(data, (sizeof(data) - 1), 1, binario);
			if(strncmp("dataAtiv", campo, sizeof(campo)) == 0 && strncmp(data, chave, sizeof(chave)) == 0) {
				printRegister = 1;
			}

			fread(uf, (sizeof(uf) - 1), 1, binario);
			if(strncmp("uf", campo, sizeof(campo)) == 0 && strncmp(uf, chave, sizeof(chave)) == 0) {
				printRegister = 1;
			}

			// ESCOLA
			fread(&campos_variaveis_size, sizeof(int), 1, binario);
			reg_size = reg_size + campos_variaveis_size;
			fread(escola, campos_variaveis_size, 1, binario);
			if(campos_variaveis_size > 0) escolaChecker = campos_variaveis_size;
			if(strncmp("nomeEscola", campo, sizeof(campo)) == 0 && strncmp(escola, chave, sizeof(chave)) == 0) {
				printRegister = 1;
			}

			// CIDADE
			fread(&campos_variaveis_size, sizeof(int), 1, binario);
			reg_size = reg_size + campos_variaveis_size;
			fread(cidade, campos_variaveis_size, 1, binario);
			if(campos_variaveis_size > 0) cidadeChecker = campos_variaveis_size;
			if(strncmp("municipio", campo, sizeof(campo)) == 0 && strncmp(cidade, chave, sizeof(chave)) == 0) {
				printRegister = 1;
			}

			// PRESTADORA
			fread(&campos_variaveis_size, sizeof(int), 1, binario);
			reg_size = reg_size + campos_variaveis_size;
			fread(prestadora, campos_variaveis_size, 1, binario);
			if(campos_variaveis_size > 0) prestadoraChecker = campos_variaveis_size;
			if(strncmp("prestadora", campo, sizeof(campo)) == 0 && strncmp(prestadora, chave, sizeof(chave)) == 0) {
				printRegister = 1;
			}

			if(printRegister) {
				// PRINT_OUTPUT
				printf("%d ", codigoINEP);
				if(data[0] != '0') printf("%s ", data);
				if(uf[0] != '0') printf("%s ", uf);
				if(escolaChecker > 0) printf("%d %s ", escolaChecker, escola);
				if(cidadeChecker > 0) printf("%d %s ", cidadeChecker, cidade);
				if(prestadoraChecker > 0) printf("%d %s", prestadoraChecker, prestadora);
				printf("\n");
				printRegister = 0;
			}
		}
		else {
			fseek(binario, IN_DISK_REG_SIZE - sizeof(int), SEEK_CUR);
		}
		if(feof(binario) == 0) {
			if (reg_size < IN_DISK_REG_SIZE) {
				fseek(binario, IN_DISK_REG_SIZE - reg_size, SEEK_CUR);
			}
		}
		else {
			break;
		}
		reg_size = 28;
	}

	rewind(binario);
	binario_h.status = '1';
	fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
	fclose(binario);
}

void file_delete_record(const char *nome_arq_binario, int rrn)
{
	if(nome_arq_binario != NULL)
	{
		HEADER binario_h;
		FILE *binario = NULL;
		int codigoINEP = 0, marca = -1;
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			binario_h.status = '0';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fread(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
			fseek(binario, (rrn - 1) * IN_DISK_REG_SIZE, SEEK_CUR);
			if(fread(&codigoINEP, sizeof(codigoINEP), 1, binario) > 0)
			{
				fseek(binario, -sizeof(codigoINEP), SEEK_CUR);
				fwrite(&marca, sizeof(marca), 1, binario);
				fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
				binario_h.topoPilha = rrn;
			}
			else
			{
				printf("Registro inexistente.\n");
			}
			binario_h.status = '1';
			rewind(binario);
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
			fclose(binario);
			printf("Registro removido com sucesso.\n");
		}
		else
		{
			printf("Falha no processamento do arquivo.\n");
		}
	}
	else
	{
		printf("Falha no processamento do arquivo.\n");
	}
}

void file_update_rrn(const char *nome_arq_binario, int rrn, int newCodigoINEP, char *newData, char *newUF, char *newEscola, char *newCidade, char *newPrestadora){
	char status = '0', bytePadding = '0';
	FILE *binario = NULL;
	int campos_variaveis_size, isRemoved, regsize;

	binario = fopen(nome_arq_binario, "w+b");
	if(binario != NULL){
		fwrite(&status, sizeof(status), 1, binario);
		fseek(binario, (IN_DISK_REG_SIZE * (rrn - 1)) + sizeof(int), SEEK_CUR);
		if(fread(&isRemoved, sizeof(int), 1, binario) > 0){
			if(isRemoved != -1){
				fseek(binario, -sizeof(isRemoved), SEEK_CUR);
				regsize = 28;
				fwrite(&newCodigoINEP, sizeof(newCodigoINEP), 1, binario);
				if(strcmp(newData, "0") == 0) fwrite("0000000000", sizeof(char), 10, binario);
				else fwrite(newData, (sizeof(newData) - 1), 1, binario);
				if(strcmp(newUF, "0") == 0) fwrite("00", sizeof(char), 2, binario);
				else fwrite(newUF, (sizeof(newUF) - 1), 1, binario);
				campos_variaveis_size = strlen(newEscola);
				regsize += campos_variaveis_size;
				fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
				if(campos_variaveis_size > 0) fwrite(newEscola, campos_variaveis_size, 1, binario);
				campos_variaveis_size = strlen(newCidade);
				regsize += campos_variaveis_size;
				fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
				if(campos_variaveis_size > 0) fwrite(newCidade, campos_variaveis_size, 1, binario);
				campos_variaveis_size = strlen(newPrestadora);
				regsize += campos_variaveis_size;
				fwrite(&campos_variaveis_size, sizeof(int), 1, binario);
				if(campos_variaveis_size > 0) fwrite(newPrestadora, campos_variaveis_size, 1, binario);
				if(regsize < 87) fwrite(&bytePadding, sizeof(char), IN_DISK_REG_SIZE - regsize, binario);
				printf("Registro alterado com sucesso.\n");
			}
			else{
				printf("Registro inexistente.\n");
			}
		}
		rewind(binario);
		fwrite(&status, sizeof(status), 1, binario);
		fclose(binario);
	}
	else{
		printf("Falha no processamento do arquivo.\n");
	}
}

void file_print_stack(const char *nome_arq_binario)
{
	if(nome_arq_binario != NULL)
	{
		HEADER binario_h;
		FILE *binario = NULL;
		int tmp_pilha = -1, marca = -1;
		binario = fopen(nome_arq_binario, "r+b");
		if(binario != NULL)
		{
			binario_h.status = '0';
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fread(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
			tmp_pilha = binario_h.topoPilha;
			if(tmp_pilha != -1)
			{
				while(tmp_pilha != -1)
				{
					printf("%d ", tmp_pilha);
					fseek(binario, ((tmp_pilha - 1) * IN_DISK_REG_SIZE) + IN_DISK_HEADER_SIZE, SEEK_SET);
					fread(&marca, sizeof(marca), 1, binario);
					fread(&tmp_pilha, sizeof(tmp_pilha), 1, binario);
				}
			}
			else
			{
				printf("Pilha vazia.");
			}
			printf("\n");
			binario_h.status = '1';
			rewind(binario);
			fwrite(&binario_h.status, sizeof(binario_h.status), 1, binario);
			fwrite(&binario_h.topoPilha, sizeof(binario_h.topoPilha), 1, binario);
			fclose(binario);
		}
		else
		{
			printf("Falha no processamento do arquivo.\n");
		}
	}
	else
	{
		printf("Falha no processamento do arquivo.\n");
	}
}
