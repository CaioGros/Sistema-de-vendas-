#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MAX 100

typedef struct {
    int SKU;
    int id;
    char nome[100];
    float preco;
    int estoque;
    int estoque_minimo; // Novo campo para estoque mínimo desejado
} produto; //estrutura modificada

typedef struct {
    int numero_pedido;
    float valor_compra;
    float valor_desconto;
    float valor_final;
    // Informações do produto vendido
    int id_produto;
    char nome_produto[100];
    float preco_produto;
    //faturamento
    float valor;
} venda; //estrutura modificada

typedef struct {
    int id;
    char nome[100];
    char email[50];
} cliente; //estrutura adicionada

typedef struct {
    int id_produto;
    int quantidade;
} ItemCarrinho;

// Função para listar produtos
void visualizarFaturamento(venda vendas[], int contador_venda);

// Função para listar produtos
void listarProdutos(produto produtos[], int contador_produto) {
    clear();
    mvprintw(1, 5, "Lista de Produtos\n\n");
    for (int i = 0; i < contador_produto; i++) {
        // Verifica se o estoque está abaixo do mínimo desejado (20 neste caso)
        if (produtos[i].estoque < 20) {
            attron(A_BOLD); // Define texto em negrito
            mvprintw(i + 2, 5, "SKU: %d | ID: %d | Nome: %s | Preço: %.2f | Estoque: %d | ESTOQUE BAIXO", produtos[i].SKU, produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].estoque);
            attroff(A_BOLD); // Desfaz negrito
        } else {
            mvprintw(i + 2, 5, "SKU: %d | ID: %d | Nome: %s | Preço: %.2f | Estoque: %d\n", produtos[i].SKU, produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].estoque);
        }
    }
    refresh();
    getch();
}

float calcularDesconto(float total) {
    if (total >= 500) {
        total -= total * 0.20;
    } else if (total >= 150) {
        total -= total * 0.15;
    }
    return total;
}


// Função para adicionar produto ao carrinho
void adicionarAoCarrinho(ItemCarrinho carrinho[], produto produtos[], int *numItens, int produtoId, int quantidade) {
    // Verifica se o produto está disponível no estoque
    for (int i = 0; i < *numItens; i++) {
        if (carrinho[i].id_produto == produtoId) {
            carrinho[i].quantidade += quantidade;
            mvprintw(13, 5, "Quantidade atualizada no carrinho.\n");
            refresh();
            getch();
            return;
        }
    }

    // Verifica se o produto existe e se há estoque disponível
    for (int i = 0; i < MAX; i++) {
        if (produtos[i].id == produtoId) {
            if (produtos[i].estoque >= quantidade) {
                carrinho[*numItens].id_produto = produtoId;
                carrinho[*numItens].quantidade = quantidade;
                (*numItens)++;
                mvprintw(19, 5, "Produto adicionado ao carrinho.\n");
            } else {
                mvprintw(19, 5, "Produto não pode ser adicionado - Estoque insuficiente.\n");
            }
            refresh();
            getch();
            return;
        }
    }

    mvprintw(19, 5, "Produto não encontrado.\n");
    refresh();
    getch();
}
// Função para exibir carrinho
void exibirCarrinho(ItemCarrinho carrinho[], int numItens, produto produtos[], int contador_produto) {
    clear();
    mvprintw(0, 5, "Carrinho de Compras\n\n");
    float valor_total_sem_desconto = 0.0;
    for (int i = 0; i < numItens; i++) {
        for (int j = 0; j < contador_produto; j++) {
            if (produtos[j].id == carrinho[i].id_produto) {
                mvprintw(i + 2, 5, "ID: %d - Nome: %s - Quantidade: %d - Preço Unitário: %.2f\n", produtos[j].id, produtos[j].nome, carrinho[i].quantidade, produtos[j].preco);
                valor_total_sem_desconto += produtos[j].preco * carrinho[i].quantidade;
                break;
            }
        }
    }

    float valor_com_desconto = calcularDesconto(valor_total_sem_desconto);
    float valor_do_desconto = valor_total_sem_desconto - valor_com_desconto;

    mvprintw(numItens + 3, 5, "Valor Total (sem desconto): %.2f\n", valor_total_sem_desconto);
    mvprintw(numItens + 4, 5, "Valor do Desconto: %.2f\n", valor_do_desconto);
    mvprintw(numItens + 5, 5, "Valor Final a ser Pago: %.2f\n", valor_com_desconto);
    refresh();
    getch();
}

// Função para editar produto no carrinho
void editarCarrinho(ItemCarrinho carrinho[], produto produtos[], int numItens) {
    int id, quantidade;
    mvprintw(numItens + 14, 5, "Digite o ID do produto que deseja editar: ");
    echo();
    scanw("%d", &id);
    noecho();
    mvprintw(numItens + 15, 5, "Digite a nova quantidade: ");
    echo();
    scanw("%d", &quantidade);
    noecho();

    for (int i = 0; i < numItens; i++) {
        if (carrinho[i].id_produto == id) {
            carrinho[i].quantidade = quantidade;
            mvprintw(numItens + 16, 5, "Quantidade atualizada no carrinho.\n");
            refresh();
            getch();
            return;
        }
    }

    mvprintw(numItens + 17, 5, "Produto não encontrado no carrinho.\n");
    refresh();
    getch();
}

// Função para deletar produto do carrinho
void deletarDoCarrinho(ItemCarrinho carrinho[], int *numItens, int produtoId, produto produtos[]) {
    for (int i = 0; i < *numItens; i++) {
        if (carrinho[i].id_produto == produtoId) {
            for (int j = i; j < (*numItens) - 1; j++) {
                carrinho[j] = carrinho[j + 1];
            }
            (*numItens)--;
            mvprintw(14, 5, "Produto removido do carrinho.\n");
            refresh();
            getch();
            return;
        }
    }
    mvprintw(14, 5, "Produto não encontrado no carrinho.\n");
    refresh();
    getch();
}

// Função para cadastrar clientes
void cadastrarCliente() {
    cliente c;
    clear();
    mvprintw(1, 5, "Cadastro de Cliente\n\n");

    mvprintw(2, 5, "Digite o ID do cliente: ");
    echo();
    scanw("%d", &c.id);
    noecho();

    mvprintw(4, 5, "Digite o nome do cliente: ");
    echo();
    getnstr(c.nome, 50);
    noecho();

    mvprintw(6, 5, "Cliente cadastrado com sucesso!\n");
    refresh();
    getch();
}

// Função para cadastrar vendas
void cadastrarVenda(venda vendas[], int *contador_venda, produto produtos[], ItemCarrinho carrinho[], float valor_total, int numItens, int numero_pedido, int contador_produto) {
    venda v;
    v.numero_pedido = numero_pedido;

    // Calcular o valor total com desconto
    float valor_com_desconto = calcularDesconto(valor_total);
    float valor_do_desconto = valor_total - valor_com_desconto;

    // Armazenar informações da venda
    v.valor_compra = valor_total;
    v.valor_desconto = valor_do_desconto;
    v.valor_final = valor_com_desconto;

    // Armazenar informações do produto vendido
    for (int i = 0; i < numItens; i++) {
        for (int j = 0; j < contador_produto; j++) {
            if (produtos[j].id == carrinho[i].id_produto) {
                //v.id_produto = produtos[j].id;
                //strcpy(v.nome_produto, produtos[j].nome);
                v.preco_produto = produtos[j].preco;
                break;
            }
        }
    }

    // Exibir as informações da venda
   // clear();
    mvprintw(1, 5, "Detalhes da Venda\n\n");
    exibirCarrinho(carrinho, numItens, produtos, &contador_produto);
   // mvprintw(2, 5, "Produto: %s (ID: %d)\n", v.nome_produto, v.id_produto);
   // mvprintw(3, 5, "Preço Unitário: %.2f\n", v.preco_produto);
    mvprintw(4, 5, "Valor da Compra: %.2f\n", v.valor_compra);
    mvprintw(5, 5, "Valor do Desconto: %.2f\n", v.valor_desconto);
    mvprintw(6, 5, "Valor Final da Compra: %.2f\n", v.valor_final);
    refresh();
    getch();

    // Registrar a venda com desconto aplicado
    vendas[*contador_venda] = v;
    (*contador_venda)++;

    // Atualiza o estoque
    for (int i = 0; i < numItens; i++) {
        for (int j = 0; j < contador_produto; j++) {
            if (produtos[j].id == carrinho[i].id_produto) {
                produtos[j].estoque -= carrinho[i].quantidade;
            }
        }
    }

    mvprintw(19, 5, "Venda cadastrada com sucesso!\n");
    refresh();
    getch();
}


// Função para calcular imposto sobre o faturamento
float calcularImposto(float faturamento) {
    float imposto = faturamento * 0.08;
    return imposto;
}

// Função para visualizar vendas, faturamento e imposto
void visualizarVendas(venda vendas[], int contador_venda) {
    float faturamento = 0.0;
    clear(); // Limpa a tela antes de imprimir as vendas

    // Imprime cada venda e calcula o faturamento total
    mvprintw(2, 5, "Visualizar Vendas\n\n");
    for (int i = 0; i < contador_venda; i++) {
        mvprintw(i + 4, 5, "ID DA VENDA: %d  - Valor total: R$ %.2f\n", vendas[i].numero_pedido, vendas[i].valor);
        faturamento += vendas[i].valor;
    }

    // Calcula o imposto sobre o faturamento
    float imposto = calcularImposto(faturamento);

    // Imprime o faturamento total e o imposto
    mvprintw(contador_venda + 6, 5, "FATURAMENTO: R$ %.2f\n", faturamento);
    mvprintw(contador_venda + 7, 5, "Imposto: R$ %.2f\n", imposto);

    refresh(); // Atualiza a tela para exibir as informações
    getch();   // Aguarda o usuário pressionar uma tecla para continuar
}


// Função para cadastro de produtos
void cadastroProduto(produto produtos[], int *contador_produto) {
    if (*contador_produto >= MAX) {
        mvprintw(15, 5, "Limite de produtos atingido.\n");
        refresh();
        getch();
        return;
    }

    produto p;
    clear();
    mvprintw(1, 5, "Cadastro de Produto\n\n\n\n");

    // Gera SKU aleatório
    srand(time(NULL));
    p.SKU = rand() % 10000;  // Exemplo: gera um número aleatório entre 0 e 9999

    // Gera ID automático
    p.id = *contador_produto + 1;  // Começa em 1 e vai aumentando conforme novos produtos são cadastrados

    mvprintw(3, 5, "SKU gerado automaticamente: %d\n", p.SKU);

    mvprintw(4, 5, "Digite o nome do produto: ");
    echo();
    getnstr(p.nome, 50);
    noecho();

    mvprintw(5, 5, "Digite o preço do produto: ");
    echo();
    scanw("%f", &p.preco);
    noecho();

    mvprintw(6, 5, "Digite a quantidade em estoque: ");
    echo();
    scanw("%d", &p.estoque);
    noecho();

    produtos[*contador_produto] = p;
    (*contador_produto)++;

    mvprintw(10, 5, "Produto cadastrado com sucesso!\n");
    refresh();
    getch();
}
// Função para editar produtos
void editarProduto(produto produtos[], int contador_produto) {
    int id;
   // clear();
    mvprintw(2, 5, "Editar Produto\n");
    mvprintw(4, 5, "Digite o ID do produto que deseja editar: ");
    echo();
    scanw("%d", &id);
    noecho();

    for (int i = 0; i < contador_produto; i++) {
        if (produtos[i].id == id) {
            mvprintw(5, 5, "Produto encontrado: %s\n", produtos[i].nome);
            mvprintw(6, 5, "Digite o novo nome do produto: ");
            echo();
            getnstr(produtos[i].nome, 50);
            noecho();

            mvprintw(7, 5, "Digite o novo preço do produto: ");
            echo();
            scanw("%f", &produtos[i].preco);
            noecho();

            mvprintw(8, 5, "Digite a nova quantidade em estoque: ");
            echo();
            scanw("%d", &produtos[i].estoque);
            noecho();

            mvprintw(9, 5, "Produto editado com sucesso!\n");
            refresh();
            getch();
            return;
        }
    }

    mvprintw(4, 5, "Produto não encontrado.\n");
    refresh();
    getch();
}

// Função para excluir produtos
void excluirProduto(produto produtos[], int *contador_produto) {
    int id;
    //clear();
    mvprintw(2, 5, "Excluir Produto\n\n");
    mvprintw(4, 5, "Digite o ID do produto que deseja excluir: ");
    echo();
    scanw("%d", &id);
    noecho();

    for (int i = 0; i < *contador_produto; i++) {
        if (produtos[i].id == id) {
            for (int j = i; j < (*contador_produto) - 1; j++) {
                produtos[j] = produtos[j + 1];
            }
            (*contador_produto)--;
            mvprintw(4, 5, "Produto excluído com sucesso.\n");
            refresh();
            getch();
            return;
        }
    }

    mvprintw(4, 5, "Produto não encontrado.\n");
    refresh();
    getch();
}


void listarVendas(venda vendas[], int contador_venda) {
    clear();
    mvprintw(1, 5, "Lista de Vendas\n\n");

    for (int i = 0; i < contador_venda; i++) {
        mvprintw(2 + i * 6, 5, "Número do Pedido: %d\n", vendas[i].numero_pedido);
       // mvprintw(3 + i * 6, 5, "Produto: %s (ID: %d)\n", vendas[i].nome_produto, vendas[i].id_produto);
      //  mvprintw(4 + i * 6, 5, "Preço Unitário: %.2f\n", vendas[i].preco_produto);
        mvprintw(5 + i * 6, 5, "Valor da Compra: %.2f\n", vendas[i].valor_compra);
        mvprintw(6 + i * 6, 5, "Valor do Desconto: %.2f\n", vendas[i].valor_desconto);
        mvprintw(7 + i * 6, 5, "Valor Final da Compra: %.2f\n", vendas[i].valor_final);
        mvprintw(8 + i * 6, 5, "-------------------------------------\n");
    }

    refresh();
    getch();
}

// Menu de administrador
void menuAdm(produto produtos[], int *contador_produto, venda vendas[], int *contador_venda) {
    int opcao;
    do {
        clear();
        mvprintw(1, 5, "Menu Administrador\n\n");
        mvprintw(2, 5, "1. Cadastrar Produto\n");
        mvprintw(3, 5, "2. Listar Produtos\n");
        mvprintw(4, 5, "3. Editar Produto\n");
        mvprintw(5, 5, "4. Excluir Produto\n");
        mvprintw(6, 5, "5. Visualizar Vendas\n");
        mvprintw(7, 5, "6. Visualizar Faturamento\n"); // Opção adicionada para visualizar faturamento
        mvprintw(8, 5, "0. Voltar\n");
        mvprintw(12, 5, "Escolha uma opção: ");
        refresh();
        echo();
        scanw("%d", &opcao);
        noecho();

        switch (opcao) {
            case 1:
                cadastroProduto(produtos, contador_produto);
                break;
            case 2:
                listarProdutos(produtos, *contador_produto);
                break;
            case 3:
            	listarProdutos(produtos, *contador_produto);
                editarProduto(produtos, *contador_produto);
                break;
            case 4:
            	listarProdutos(produtos, *contador_produto);
                excluirProduto(produtos, contador_produto);
                break;
            case 5:
                listarVendas(vendas, *contador_venda);
                break;
            case 6:
                visualizarFaturamento(vendas, *contador_venda); // Chama a função para visualizar faturamento
                break;
            case 0:
                return;
            default:
                mvprintw(9, 5, "Opção inválida. Tente novamente.\n");
                refresh();
                getch();
                break;
        }
    } while (opcao != 0);
}

// Menu de usuário
void menuUsuario(produto produtos[], int *contador_produto, venda vendas[], int *contador_venda) {
    int opcao, produtoId, quantidade, numero_pedido;
    float valor_total = 0.0;
    ItemCarrinho carrinho[MAX];
    int numItens = 0;

    do {
        clear();
        mvprintw(1, 5, "Menu Usuário\n\n");

        // Listar produtos automaticamente
        listarProdutos(produtos, *contador_produto);
        mvprintw(4, 5,"--------------------------------------------------------------\n");
        mvprintw(*contador_produto + 5, 5, "1. Adicionar Produto ao Carrinho\n");
        mvprintw(*contador_produto + 6, 5, "2. Exibir Carrinho\n");
        mvprintw(*contador_produto + 7, 5, "3. Editar Carrinho\n");
        mvprintw(*contador_produto + 8, 5, "4. Deletar Carrinho\n");
        mvprintw(*contador_produto + 9, 5, "5. Finalizar Venda\n");
        mvprintw(*contador_produto + 10, 5, "0. Voltar\n");
        mvprintw(*contador_produto + 12, 5, "Escolha uma opção: ");
        refresh();
        echo();
        scanw("%d", &opcao);
        noecho();

        switch (opcao) {
            case 1:
                mvprintw(*contador_produto + 13, 5, "Digite o ID do produto: ");
                echo();
                scanw("%d", &produtoId);
                noecho();
                mvprintw(*contador_produto + 14, 5, "Digite a quantidade: ");
                echo();
                scanw("%d", &quantidade);
                noecho();
                adicionarAoCarrinho(carrinho, produtos, &numItens, produtoId, quantidade);
                break;
            case 2:
                exibirCarrinho(carrinho, numItens, produtos, *contador_produto);
                break;
            case 3:
            	exibirCarrinho(carrinho, numItens, produtos, *contador_produto);
                editarCarrinho(carrinho, produtos, numItens);
                break;
            case 4:
            	exibirCarrinho(carrinho, numItens, produtos, *contador_produto);
                mvprintw(*contador_produto + 13, 5, "Digite o ID do produto que deseja deletar: ");
                echo();
                scanw("%d", &produtoId);
                noecho();
                deletarDoCarrinho(carrinho, &numItens, produtoId, produtos);
                break;
            case 5:
                valor_total = 0.0;
                for (int i = 0; i < numItens; i++) {
                    for (int j = 0; j < *contador_produto; j++) {
                        if (produtos[j].id == carrinho[i].id_produto) {
                            valor_total += produtos[j].preco * carrinho[i].quantidade;
                        }
                    }
                }
                mvprintw(*contador_produto + 17, 5, "Digite o número do pedido: ");
                echo();
                scanw("%d", &numero_pedido);
                noecho();
               // exibirCarrinho(carrinho, numItens, produtos, *contador_produto);
                cadastrarVenda(vendas, contador_venda, produtos, carrinho, valor_total, numItens, numero_pedido, *contador_produto);
                numItens = 0; // Limpa o carrinho após a venda
                break;
            case 0:
                return;
            default:
                mvprintw(*contador_produto + 16, 5, "Opção inválida. Tente novamente.\n");
                refresh();
                getch();
                break;
        }
    } while (opcao != 0);
}

// Tela inicial
void telaInicial(produto produtos[], int *contador_produto, venda vendas[], int *contador_venda) {
    int opcao;
    do {
        clear();
        mvprintw(1, 5, "Bem-vindo ao Puro Voodoo\n\n");
        mvprintw(3, 5, "1. Administrador\n");
        mvprintw(4, 5, "2. Usuário\n");
        mvprintw(5, 5, "3. Cadastrar Cliente\n");
        mvprintw(6, 5, "0. Sair\n");
        mvprintw(8, 5, "Escolha uma opção: ");
        refresh();
        echo();
        scanw("%d", &opcao);
        noecho();

        switch (opcao) {
            case 1:
                menuAdm(produtos, contador_produto, vendas, contador_venda);
                break;
            case 2:
                menuUsuario(produtos, contador_produto, vendas, contador_venda);
                break;
            case 3:
                cadastrarCliente();
                break;
            case 0:
                return;
            default:
                mvprintw(7, 5, "Opção inválida. Tente novamente.\n");
                refresh();
                getch();
                break;
        }
    } while (opcao != 0);
}

void visualizarFaturamento(venda vendas[], int contador_venda) {
    float faturamento = 0.0;
    for (int i = 0; i < contador_venda; i++) {
         faturamento += vendas[i].valor_final;
    }
    float imposto = calcularImposto(faturamento);

    clear(); // Limpa a tela
    mvprintw(2, 5, "FATURAMENTO TOTAL\n\n");
    for (int i = 0; i < contador_venda; i++) {
        mvprintw(i + 4, 5, "ID DA VENDA: %d  - Valor total: R$ %.2f\n", vendas[i].numero_pedido, vendas[i].valor);
    }
    mvprintw(contador_venda + 3, 5, "faturamento: R$ %.2f\n", faturamento);
    mvprintw(contador_venda + 4, 5, "Imposto: R$ %.2f\n", imposto);
    refresh(); // Atualiza a tela para exibir as informações
    getch(); // Aguarda o usuário pressionar uma tecla para continuar
}

// Função principal
int main() {
    initscr();
    noecho();
    cbreak();

    produto produtos[MAX];
    venda vendas[MAX];
    int contador_produto = 0;
    int contador_venda = 0;

    telaInicial(produtos, &contador_produto, vendas, &contador_venda);

    endwin();
    return 0;
}