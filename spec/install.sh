#!/bin/bash

GREEN='\033[0;32m'
BLUE='\033[0;36m'
RED='\033[0;31m'
WHITE='\033[0m'

# Renders a text based list of options that can be selected by the
# user using up, down and enter keys and returns the chosen option.
#
#   Arguments   : list of options, maximum of 256
#                 "opt1" "opt2" ...
#   Return value: selected index (0 for opt1, 1 for opt2 ...)
select_option() {

    # little helpers for terminal print control and key input
    ESC=$( printf "\033")
    cursor_blink_on()  { printf "$ESC[?25h"; }
    cursor_blink_off() { printf "$ESC[?25l"; }
    cursor_to()        { printf "$ESC[$1;${2:-1}H"; }
    print_option()     { printf "   $BLUE$1$WHITE "; }
    print_selected()   { printf "   $ESC[7m$1$ESC[27m"; }
    get_cursor_row()   { IFS=';' read -sdR -p $'\E[6n' ROW COL; echo ${ROW#*[}; }
    key_input()        { read -s -n3 key 2>/dev/null >&2
                         if [[ $key = $ESC[A ]]; then echo up;    fi
                         if [[ $key = $ESC[B ]]; then echo down;  fi
                         if [[ $key = ""     ]]; then echo enter; fi; }

    # initially print empty new lines (scroll down if at bottom of screen)
    for opt; do printf "\n"; done

    # determine current screen position for overwriting the options
    local lastrow=`get_cursor_row`
    local startrow=$(($lastrow - $#))

    # ensure cursor and input echoing back on upon a ctrl+c during read -s
    trap "cursor_blink_on; stty echo; printf '\n'; exit" 2
    cursor_blink_off

    local selected=0
    while true; do
        # print options by overwriting the last lines
        local idx=0
        for opt; do
            cursor_to $(($startrow + $idx))
            if [ $idx -eq $selected ]; then
                print_selected "$opt"
            else
                print_option "$opt"
            fi
            ((idx++))
        done

        # user key control
        case `key_input` in
            enter) break;;
            up)    ((selected--));
                   if [ $selected -lt 0 ]; then selected=$(($# - 1)); fi;;
            down)  ((selected++));
                   if [ $selected -ge $# ]; then selected=0; fi;;
        esac
    done

    # cursor position back to normal
    cursor_to $lastrow
    printf "\n"
    cursor_blink_on

    return $selected
}

install_brew()
{
    brew --version &>/dev/null && echo "brew already installed" || \
    ( \
        ( \
            mv $HOME/.brew $HOME/goinfre/.brew &>/dev/null || \
            rm -rf $HOME/.brew && rm -rf $HOME/goinfre/.brew \
        ) && \
        git clone --depth=1 https://github.com/Homebrew/brew $HOME/goinfre/.brew && \
        brew --version &>/dev/null || \
        ( \
            echo 'export PATH=$HOME/goinfre/.brew/bin:$PATH' >> $HOME/.zshrc && \
            source $HOME/.zshrc \
        ) && \
        echo "brew installed" \
    )
}

install_cmake()
{
	cmake -version &>/dev/null && echo "cmake already installed" || \
	( \
		brew install cmake && \
		echo "cmake installed" \
	)
}

install_brew_package()
{
	@brew list $1 &>/dev/null && echo "$1 already installed" || \
	( \
		brew install $1 && \
		echo "$1 installed" \
	)
}

while true
do
    echo -e $BLUE"What do you want to install?"$WHITE
    options=("brew inside goinfre (please don't run this if you don't want to install brew in goinfre)" "cmake" "glfw" "googletest" "quit")

    select_option "${options[@]}"
    case $? in
    0)
        install_brew
        ;;
    1)
        install_cmake
        ;;
    2)
        install_brew_package glfw
        ;;
    3)
        install_brew_package googletest
        ;;
    *) 
        break ;;
    esac
done