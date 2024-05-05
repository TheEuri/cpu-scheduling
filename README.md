## Sobre

Código desenvolvido na cadeira de Infraestrutura de Software do 3º período do curso de ciência da computação da CESAR School.

O objetivo da atividade era passar nos testes e cumprir com os requisitos descritos no documento CPUScheduling.pdf enviado pelo professor e enviar até o deadline.

O código presente neste repositório passou com 100% em todos os testes, porém sei que não está otimizado, mas está funcional ;)

O código simula o funcionamento de 2 algoritimos de escalonamento de CPU, o Rate Monotonic e o Earliest Deadline First.

## Para rodar

Primeiro faça o make do algoritimo desejado: `make rate` ou `make edf`

Depois rode em ambiente linux passando o arquivo de teste: `./rate teste.txt` ou `./edf teste.txt` ou se preferir execute o teste.sh para rodar os testes para validar a implementação.
