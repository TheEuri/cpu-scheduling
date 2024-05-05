#!/bin/bash

# Função para executar testes
run_tests() {
  command=$1
  echo "---------------------------------"
  echo "Executando testes para $command"
  echo "---------------------------------"

  # Loop through the input files
  for inputfile in ./tests/$command/input/*.txt
  do
    # Extrair o número do arquivo de entrada
    filenumber=$(basename $inputfile .txt)

    comparefile="./tests/$command/out/$filenumber.out"

    # Executar o comando no arquivo de entrada
    ./$command $inputfile

    # Comparar a saída com o arquivo comparefile
    total_lines=$(wc -l < "$comparefile")
    diff_lines=$(diff -y --suppress-common-lines $command.out "$comparefile" | wc -l)
    same_lines=$((total_lines - diff_lines))
    
    # Calcular a porcentagem de similaridade
    if [ $total_lines -gt 0 ]
    then
      similarity=$((100 * same_lines / total_lines))
    else
      similarity=0
    fi

    if diff -a $command.out "$comparefile" &> /dev/null
    then
      echo "Teste $filenumber: Passou"
      echo "---------------------------------"
    else
      echo "Teste $filenumber: Não passou"
      echo
      # print the diff
      diff -a $command.out "$comparefile"
      echo
      echo "Similaridade: $similarity%"
      echo "---------------------------------"
    fi
  done
}

# Executar testes para rate e edf sequencialmente
run_tests rate
run_tests edf