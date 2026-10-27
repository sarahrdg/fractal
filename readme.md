# MPI

## Pré-requisitos

### Instalar openmpi 

```bash
sudo apt update
sudo apt install openmpi-bin libopenmpi-dev
```

### Intslar ferramentas básicas

```bash
sudo apt install build-essential
```

### Instalar dependências
```bash
 sudo apt install libx11-dev
 sudo apt install zlib1g-dev
```

## Como compilar e executar em UM pc

### 1. Limpar arquivos gerados anteriormente

```bash
make clean
```

### 2. Compilar o projeto

```bash
make
```

### 3. Executar 

```bash
mpirun -np 4 ./mandelbrot
```

## Pra rodar nos 3 computadores

### 1. Precisa ter
- A pasta com o projeto precisa ter o nome exatamente igual a **mandelbrot-default** e precisa estar na pasta C, o caminho deverá ser exatamente esse:
```bash 
/mnt/c/mandelbrot-default 
```

- Projeto compilado

```bash
make clean
make
```

- Firewall desativado

No terminal de administrador, rode:
```bash 
Get-NetFirewallProfile | Format-Table Name, Enabled
```

Se todos forem false ok, se tiver pelo menos um true:
```bash 
 netsh advfirewall set allprofiles state off
```

Para reativar após a prática:
```bash 
 netsh advfirewall set allprofiles state on
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

Pra ativar a ssh:

```bash
sudo systemctl start ssh
```

- PC mestre:

#### Criar a chave SSH

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
