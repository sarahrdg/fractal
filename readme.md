# MPI


## Como compilar e executar em UM pc

### 1. Instala as dependencias 
```bash
 sudo apt install libx11-dev
 sudo apt install zlib1g-dev
```

### 2. Limpar arquivos gerados anteriormente

```bash
make clean
```

### 3. Compilar o projeto

```bash
make
```

### 4. Executar 

```bash
mpirun -np 4 ./mandelbrot
```




## Pra rodar nos 3 computadores

### 1. Precisa ter

- MPI

```bash
sudo apt update
sudo apt install openmpi-bin libopenmpi-dev
```

- A pasta com o projeto precisa ter o nome exatamente igual a **mandelbrot-default** e precisa estar na pasta C, de forma que o caminho no terminal seja exatamente esse:
```bash 
/mnt/c/mandelbrot-default 
```

- No pc da faculdade:
```bash 
sudo mkdir -p /mnt/c/mandelbrot-default

cp -r /caminho/do/projeto/* /mnt/c/mandelbrot-default/

cd /mnt/c/mandelbrot-default
```

- Dependências instaladas

```bash
 sudo apt install libx11-dev
 sudo apt install zlib1g-dev
```

 - Projeto compilado

```bash
git clone bla bla bla
make clean
make
```

### 2. Configurar SSH

- PC escravos:

#### Instalar servidor SSH

```bash
sudo apt update
sudo apt install openssh-server
```
#### Verifica se deu certo

```bash
sudo systemctl status ssh
```

É pra aparecer:

```bash
active (running)
```

- PC mestre:

#### Criar a SSH

```bash
ssh-keygen -t rsa
```

É pra aparecer:

```bash
Enter file in which to save the key (/home/thais/.ssh/id_rsa):
```

- Se pedir senha só apertar enter 2 vezes

#### Enviar SSH pra outra máquina

```bash
ssh-copy-id usuario@IP_da_outra_maquina
```

#### Testar:

```bash
ssh usuario@192.168.0.20
```

Se entrar sem pedir senha, ta certo

Pra sair:

```bash
exit
```


### 3. Colocar os ips das maquinas no arquivo hosts no pc do mestre

### 4. Rodar
```bash
git clone bla bla bla
mpirun -np 4 --hostfile hosts ./mandelbrot
```


- Caso o pc do lab nao inicie a ssh:
sudo systemctl start ssh
