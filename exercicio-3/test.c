#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS_CONS 6 // qtd de consumidores

// Tipos
typedef struct {
    long long prime_cnt;
    long int winner;
    int status;
} pc_t;

typedef struct {
    long long val;
} ll;

// Variáveis
long long n; // qtd de números a gerar
int m; // capacidade do buffer
long long s0; // elemento inicial da sequência

long long *buffer;
int l, r; // [l, r) intervalo do buffer em uso
int isProducerFinished; // [produtor já finalizou]
int to_consume; // elementos a consumir do buffer

pthread_t tid[1 + NTHREADS_CONS]; // tids das threads, produtor em tid[0]
long long score[1 + NTHREADS_CONS] = {0}; // número de primos que cada thread contou

sem_t isBufferEmpty;
sem_t filledSlots;
sem_t bufferMutex;

// Input
int sanitizeInput(int argc, char** argv);
int isnumber(char* c);

// Produtor Consumidor
pc_t producerConsumer();
void* producer(void* args);
void* consumer(void* args);
void pushBuffer(long long num);
long long popBuffer();
int ehPrimo(long long int n);

int main(int argc, char** argv) {
    int status = 0;
    if(status = sanitizeInput(argc, argv)) {
        return status;
    }
    
    n = atoll(argv[1]);
    m = atoi(argv[2]);
    s0 = atoll(argv[3]);

    pc_t ans = producerConsumer();
    
    if(ans.status) {
        return ans.status;
    }

    printf("%lld\n", ans.prime_cnt);

    return 0;
}

int sanitizeInput(int argc, char** argv) {
    if(argc != 4) {
        printf("Uso: <n> <m> <inteiro inicial>\n");
        return 1;
    }

    long long n = atoll(argv[1]);
    int m = atoi(argv[2]);
    int first = atoll(argv[3]);
    
    if(!isnumber(argv[1]) || n < 0) {
        printf("ERRO! n deve ser um inteiro maior ou igual a 0.\n");
        return 1;
    }
    
    if(!isnumber(argv[2]) || m <= 0) {
        printf("ERRO! m deve ser um inteiro maior que 0.\n");
        return 1;
    }
    
    if(!isnumber(argv[3])) {
        printf("ERRO! O número inicial da sequência deve ser um inteiro\n");
        return 1;
    }

    return 0;
}

int isnumber(char* c) {
    int i = 0;
    if(c[i] == '-') c++;
    while(c[i] != '\0') {
        if(!isdigit(c[i])) return 0;
        i++;
    }
    return i > 0;
}

pc_t producerConsumer() {
    pc_t ans = {};

    l = 0, r = 0, isProducerFinished = 0, to_consume = 0;
    buffer = calloc(m, sizeof(long long));
    sem_init(&isBufferEmpty, 0, 1);
    sem_init(&filledSlots, 0, 0);
    sem_init(&bufferMutex, 0, 1);
    
    if(pthread_create(&tid[0], NULL, producer, NULL)) {
        printf("ERRO! Falha ao criar produtor.\n");
        ans.status = 1;
        return ans;
    }
    
    for(long int i = 1; i <= NTHREADS_CONS; i++) {
        if(pthread_create(&tid[i], NULL, consumer, (void*) i)) {
            printf("ERRO! Falha ao criar um consumidor.\n");
            ans.status = 2;
            return ans;
        }
    }
    
    if(pthread_join(tid[0], NULL)) {
        printf("ERRO! Falha ao finalizar produtor.\n");
        ans.status = 3;
        return ans;
    }
    
    sem_wait(&isBufferEmpty);
    
    isProducerFinished = 1;
    for(int i = 0; i < NTHREADS_CONS; i++) {
        pushBuffer(-1);
    }
    
    for(int i = 1; i <= NTHREADS_CONS; i++) {
        if(pthread_join(tid[i], NULL)) {
            printf("ERRO! Falha ao finalizar consumidor.\n");
            ans.status = 4;
            return ans;
        }
    }
    
    long long winner_primes = -1;
    for(int i = 1; i <= NTHREADS_CONS; i++) {
        ans.prime_cnt += score[i];
        if(score[i] > winner_primes) {
            ans.winner = i;
            winner_primes = score[i];
        }
    }
    
    sem_destroy(&isBufferEmpty);
    sem_destroy(&filledSlots);
    sem_destroy(&bufferMutex);
    free(buffer);
    
    return ans;
}

void* producer(void* args) {
    for(long long cnt = 0; cnt < n; cnt += m) {
        sem_wait(&isBufferEmpty);
        int mx = to_consume = (n-cnt > m) ? m : n-cnt; 
        for(int i = 0; i < mx; i++) {
            pushBuffer(s0+cnt+i);
        }
    }
    pthread_exit(NULL);
}

void* consumer(void* args) {
    int id = (long) args;
    while(1) {
        sem_wait(&filledSlots);
        if(isProducerFinished) break;
        score[id] += ehPrimo(popBuffer());
    }
    pthread_exit(NULL);
}

void pushBuffer(long long num) {
    buffer[r] = num;
    r = (r == m-1) ? 0 : r+1;
    sem_post(&filledSlots);
}

long long popBuffer() {
    sem_wait(&bufferMutex);
    long long num = buffer[l];
    l = (l == m-1) ? 0 : l+1;
    
    to_consume--;
    if(to_consume == 0) {
        sem_post(&isBufferEmpty);
    }
    sem_post(&bufferMutex);
    
    return num;
}

int ehPrimo(long long int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<sqrt(n)+1; i+=2)
        if(n%i==0) return 0;
    return 1;
}