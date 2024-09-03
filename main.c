#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// -------- Declaração dos vetores de struct's --------------------//

typedef struct Register
{
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;

} Register;

typedef struct Remove
{

    char id_aluno[4];
    char sigla_disc[4];

} Remove;

//----------- Função de Leitura dos arquivos --------------//

Register *read_insert_file()
{
    FILE *arquivo = fopen("insere.bin", "r+b");

    Register *registros = (Register *)malloc(100 * sizeof(Register));

    if (registros != NULL)
    {
        fread(registros, sizeof(struct Register), 100, arquivo);

        fclose(arquivo);
        return registros;
    }

    printf("Falha na alocação de memória para o registro de inserção\n");

    fclose(arquivo);
    return NULL;
}

Remove *read_remove_file()
{
    FILE *arquivo = fopen("remove.bin", "r+b");

    Remove *registros = (Remove *)malloc(100 * sizeof(Remove));

    if (registros != NULL)
    {

        fread(registros, sizeof(struct Remove), 100, arquivo);

        fclose(arquivo);
        return registros;
    }

    printf("Falha na alocação de memória para o registro de remoção\n");

    fclose(arquivo);
    return NULL;
}

// ---------- Função de carregamento de arquivos -------//

FILE *iniciaLogFile()
{
    FILE *log_file = fopen("log_file.bin", "r+b");

    if (log_file == NULL)
    {

        log_file = fopen("log_file.bin", "w+b");

        if (log_file != NULL)
        {
            int index = -1;

            fwrite(&index, sizeof(int), 1, log_file);
            fwrite(&index, sizeof(int), 1, log_file);
            fseek(log_file, 0, SEEK_SET);

            return log_file;
        }
        else
        {
            printf("Falha na abertura do arquivo log.\n");
            return NULL;
        }
    }

    return log_file;
}

FILE *iniciaArquivo()
{

    FILE *seuArquivo = fopen("dados.bin", "r+b");

    if (seuArquivo == NULL)
    {

        seuArquivo = fopen("dados.bin", "w+b");

        if (seuArquivo != NULL)
        {

            int byte_offset = -1;
            bool operacao_flag = false;

            fwrite(&operacao_flag, sizeof(bool), 1, seuArquivo);
            fwrite(&byte_offset, sizeof(int), 1, seuArquivo);
            fseek(seuArquivo, 0, SEEK_SET);

            return seuArquivo;
        }
        else
        {
            printf("Falha na abertura do arquivo dados.\n");
            return NULL;
        }
    }
    return seuArquivo;
}

// ------ Função de cálculo do tamanho do  registro ------//

int calcula_tamanho(Register *registro)
{

    int tam = 0;

    tam += strlen(registro->id_aluno);
    tam += strlen(registro->sigla_disc);
    tam += strlen(registro->nome_aluno);
    tam += strlen(registro->nome_disc);
    tam += sizeof(registro->media);
    tam += sizeof(registro->freq);
    tam = tam + 5;

    return tam;
}

// ------- Função de inserção de registros ------------------//

void insertRegister(Register *registro, int numb)
{
    FILE *log_file = iniciaLogFile();
    FILE *data_file = iniciaArquivo();

    char delimitador = '#';
    int i = 0, tam_reg = 0, tam_data = 0;
    int offset_last, next_byte_offset, actual_byte_offset;

    bool operation_flag, is_inserted, flag = true;
    fread(&operation_flag, sizeof(bool), 1, data_file);

    if (operation_flag)
    {
        fread(&i, sizeof(int), 1, log_file);
        i++;
    }

    fseek(data_file, 0, SEEK_SET);
    fwrite(&flag, sizeof(bool), 1, data_file);

    for (int j = i; j < i + numb; j++)
    {
        tam_reg = calcula_tamanho(&registro[j]);

        fseek(data_file, sizeof(bool), SEEK_SET);
        offset_last = ftell(data_file);
        fread(&actual_byte_offset, sizeof(int), 1, data_file);

        while (actual_byte_offset != -1)
        {
            fseek(data_file, actual_byte_offset, SEEK_SET);
            fread(&tam_data, sizeof(int), 1, data_file);
            printf("%d\n", tam_data);
            fseek(data_file, sizeof(char), SEEK_CUR);
            fread(&next_byte_offset, sizeof(int), 1, data_file);

            printf("%d\n", next_byte_offset);

            printf("%d\n", tam_reg);

            if (tam_reg <= tam_data)
            {

                fseek(data_file, actual_byte_offset + sizeof(int), SEEK_SET);

                fwrite(registro[j].id_aluno, sizeof(char), 3, data_file);
                fwrite(&delimitador, sizeof(char), 1, data_file);
                fwrite(registro[j].sigla_disc, sizeof(char), 3, data_file);
                fwrite(&delimitador, sizeof(char), 1, data_file);
                fwrite(registro[j].nome_aluno, strlen(registro[j].nome_aluno), 1, data_file);
                fwrite(&delimitador, sizeof(char), 1, data_file);
                fwrite(registro[j].nome_disc, strlen(registro[j].nome_disc), 1, data_file);
                fwrite(&delimitador, sizeof(char), 1, data_file);
                fwrite(&registro[j].media, sizeof(float), 1, data_file);
                fwrite(&delimitador, sizeof(char), 1, data_file);
                fwrite(&registro[j].freq, sizeof(float), 1, data_file);

                fseek(data_file, offset_last, SEEK_SET);
                fwrite(&next_byte_offset, sizeof(int), 1, data_file);

                fseek(log_file, 0, SEEK_SET);
                fwrite(&j, sizeof(int), 1, log_file);

                is_inserted = true;
                break;
            }

            offset_last = actual_byte_offset + sizeof(char) + sizeof(int);
            actual_byte_offset = next_byte_offset;
        }

        if (actual_byte_offset == -1)
        {
            fseek(data_file, 0, SEEK_END);

            fwrite(&tam_reg, sizeof(int), 1, data_file);
            fwrite(registro[j].id_aluno, sizeof(char), 3, data_file);
            fwrite(&delimitador, sizeof(char), 1, data_file);
            fwrite(registro[j].sigla_disc, sizeof(char), 3, data_file);
            fwrite(&delimitador, sizeof(char), 1, data_file);
            fwrite(registro[j].nome_aluno, strlen(registro[j].nome_aluno), 1, data_file);
            fwrite(&delimitador, sizeof(char), 1, data_file);
            fwrite(registro[j].nome_disc, strlen(registro[j].nome_disc), 1, data_file);
            fwrite(&delimitador, sizeof(char), 1, data_file);
            fwrite(&registro[j].media, sizeof(float), 1, data_file);
            fwrite(&delimitador, sizeof(char), 1, data_file);
            fwrite(&registro[j].freq, sizeof(float), 1, data_file);

            fseek(log_file, 0, SEEK_SET);
            fwrite(&j, sizeof(int), 1, log_file);
        }
    }

    fclose(log_file);
    fclose(data_file);

    return;
}

// ------- Função de remoção de registros ---------------//

void removeRegister(Remove *registro, int numb)
{
    // Inicia-se os arquivos

    FILE *log_file = iniciaLogFile();
    FILE *data_file = iniciaArquivo();

    bool operation_flag, is_removed = false;

    int first_stack, removed_stack;
    int i = 0;
    char ch[8], *token, marker = '*';

    fread(&operation_flag, sizeof(bool), 1, data_file);
    fread(&first_stack, sizeof(int), 1, data_file);

    if (operation_flag)
    {
        fseek(log_file, sizeof(int), SEEK_SET);
        fread(&i, sizeof(int), 1, log_file);
        i++;
    }

    for (int j = i; j < i + numb; j++)
    {

        int size;
        is_removed = false;

        fseek(data_file, sizeof(bool) + sizeof(int), SEEK_SET);

        while (!is_removed)
        {
            removed_stack = ftell(data_file);

            if (fread(&size, sizeof(int), 1, data_file) != 1)
            {
                printf("Não encontrado\n");
                break;
            }

            fread(ch, sizeof(char), 7, data_file);

            ch[7] = '\0';
            token = strtok(ch, "#");

            if (strcmp(token, registro[j].id_aluno) == 0)
            {
                token = strtok(NULL, "#");

                getchar();

                if (strcmp(token, registro[j].sigla_disc) == 0)
                {
                    fseek(data_file, removed_stack + sizeof(int), SEEK_SET);
                    fwrite(&marker, sizeof(char), 1, data_file);
                    fwrite(&first_stack, sizeof(int), 1, data_file);

                    fseek(data_file, sizeof(bool), SEEK_SET);
                    fwrite(&removed_stack, sizeof(int), 1, data_file);

                    fseek(log_file, sizeof(int), SEEK_SET);
                    fwrite(&j, sizeof(int), 1, log_file);

                    is_removed = true;
                }
            }

            fseek(data_file, size - 7, SEEK_CUR);
        }
    }

    fclose(log_file);
    fclose(data_file);

    return;
}

void compactFile()
{
    FILE *data_file = iniciaArquivo();
    FILE *compact_file = fopen("compact_file.bin", "w+b");

    int byte_offset = -1;
    bool operacao_flag = true;

    fwrite(&operacao_flag, sizeof(bool), 1, compact_file);
    fwrite(&byte_offset, sizeof(int), 1, compact_file);

    int size = 0, contador = 0, pos_inicial = 0, pos_final = 0, pos_compact = 0;
    char ch[100], hash = '#';
    float num1, num2;

    fseek(data_file, sizeof(bool) + sizeof(int), SEEK_SET);

    while (fread(&size, sizeof(int), 1, data_file) == 1)
    {

        pos_inicial = ftell(data_file);
        pos_compact = ftell(compact_file);

        fread(ch, sizeof(char), 8, data_file);

        if (ch[0] != '*')
        {
            fwrite(&size, sizeof(int), 1, compact_file);
            fwrite(ch, sizeof(char), 8, compact_file);

            contador = 0;

            while (contador < 2 && fread(ch, sizeof(char), 1, data_file) == 1)
            {
                if (ch[0] == hash)
                {
                    contador++;
                }
                fwrite(ch, sizeof(char), 1, compact_file);
            }

            if (fread(&num1, sizeof(float), 1, data_file) == 1)
            {
                fwrite(&num1, sizeof(float), 1, compact_file);
            }

            fread(ch, sizeof(char), 1, data_file);
            if (ch[0] == hash)
            {
                fwrite(&hash, sizeof(char), 1, compact_file);
            }

            if (fread(&num2, sizeof(float), 1, data_file) == 1)
            {
                fwrite(&num2, sizeof(float), 1, compact_file);
            }

            pos_final = ftell(data_file);

            int registro_size = pos_final - pos_inicial;
            size = size - registro_size;

            if (registro_size != size)
            {
                fseek(compact_file, pos_compact, SEEK_SET);
                fwrite(&registro_size, sizeof(int), 1, compact_file);
                fseek(compact_file, 0, SEEK_END);
            }

            fseek(data_file, size, SEEK_CUR);
        }
        else
        {
            fseek(data_file, size - 8, SEEK_CUR);
        }
    }

    fclose(data_file);
    fclose(compact_file);

    remove("dados.bin");
    rename("compact_file.bin", "dados.bin");

    return;
}

int main()
{
    Register *insert_register = read_insert_file();
    Remove *remove_register = read_remove_file();

    bool flag = true;
    int answr;

    while (flag)
    {

        printf("\nO que deseja fazer? \n[1]Inserir \n[2]Remover \n[3]Compactar\n[4]Remover Arquivos\n[0]Encerrar\nR:");
        scanf("%d", &answr);

        if (answr == 1)
        {
            insertRegister(insert_register, 1);
        }
        else if (answr == 2)
        {
            removeRegister(remove_register, 1);
        }
        else if (answr == 0)
        {
            printf("Programa encerrado\n");
            flag = false;
        }
        else if (answr == 3)
        {

            compactFile();
        }
        else if (answr == 4)
        {
            remove("dados.bin");
            remove("log_file.bin");
        }
        else
        {
            printf("Resposta desconhecida, tente novamente\n");
        }
    }

    free(remove_register);
    free(insert_register);

    return;
}