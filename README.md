## Sobre

Código desenvolvido na cadeira de Infraestrutura de Software do 3º período do curso de ciência da computação da CESAR School.

O objetivo da atividade era passar nos testes, cumprir com os requisitos descritos no documento CPUScheduling.pdf enviado pelo professor e enviar até o deadline.

O código presente neste repositório passou com nota máxima, porém sei que não está otimizado, mas está funcional ;)

Outros requisitos/informações dadas pelo professor que não foram descritos no documento:
```
A ordem de impressão das tarefas nas listas LOST DEADLINES, COMPLETE EXECUTION e KILLED é a mesma do arquivo de entrada.
LOST vem antes de KILL, ou seja, a verificação do LOST deve vir antes do KILL.
Finish (F), depois Hold (H) e na sequência (L)
```

O código simula o funcionamento de 2 algoritimos de escalonamento de CPU, o Rate Monotonic e o Earliest Deadline First.

## Para rodar

Primeiro faça o make do algoritimo desejado: `make rate` ou `make edf`

Depois rode em ambiente linux passando o arquivo de teste: `./rate teste.txt` ou `./edf teste.txt` ou se preferir execute o teste.sh para rodar os testes para validar a implementação.

### Intruções sobre os testes
Esses testes são para ser rodados em ambiente linux. Se você estiver no windows, você pode usar o WSL (Windows Subsystem for Linux) para rodar esses comandos.

Primeiro, certifique-se de que o arquivo tem permissões de execução. Você pode fazer isso com o comando chmod:
`chmod +x teste.sh`

Depois você deve rodar os comandos do make, para gerar os executáveis:  
`make rate`
`make edf`

Depois, você pode rodar o script com o comando:
`./teste.sh`

O SCRIPT NÃO MODIFICA NADA DO SEU CÓDIGO. Ele apenas compila e roda os testes. Se você quiser rodar os testes manualmente, você pode fazer isso com os comandos:
`./rate testes/rate/input/01.txt`
`./edf testes/edf/input/01.txt`

ou se preferir é só deixar o caso que você quer testar na pasta de input e rodar o comando `./teste.sh` normalmente.

A pasta deve ta organizada da seguinte forma:

```
.
├── Makefile
├── real.c
├── edf
├── rate
├── teste.sh
├── testes
│   ├── edf
│   │   ├── input
│   │   │   ├── 01.txt
│   │   │   ├── N.txt
│   │   ├── out
│   │   │   ├── 01.txt
│   │   │   ├── N.txt
│   ├── rate
│   │   ├── input
│   │   │   ├── 01.txt
│   │   │   ├── N.txt
│   │   ├── out
│   │   │   ├── 01.txt
│   │   │   ├── N.txt

```
