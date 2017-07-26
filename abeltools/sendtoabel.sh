#!/bin/sh

function show_help {
    echo "Nah, you're on your own"
    exit
}

# A POSIX variable
OPTIND=1         # Reset in case getopts has been used previously in the shell.

#### GET ARGUMENTS ####
# Initialize our own variables:
SOURCE='..'
DESTINATION=$2
HOST=$1
FILESFILE="filestosend.txt"
verbose=0

# Fossilkode. Fungerer fortsatt for optional parameters
while getopts "h?vf:" opt; do
    case "$opt" in
    h|\?)
        show_help
        exit 0
        ;;
    v)  verbose=1
        ;;
    f)  destination=$OPTARG
        ;;
    esac
done

shift $((OPTIND-1))

[ "$1" = "--" ] && shift

#### END GET ARGUMENTS ####

if ssh $HOST stat $DESTINATION \> /dev/null 2\>\&1
then
    # Directory exists
    echo "Folder exists. Exiting"
#    exit
else
    # Make directory. Doesn't work and isn't needed
    # ssh $HOST 'mkdir ${DESTINATION}'
    echo "Nothing"
fi


while read line; do
    if   [ -d "${SOURCE}/${line}" ]
    then # It is a directory
        echo "Dir ${line}"
        scp -r $SOURCE/$line $HOST:~/$DESTINATION/
    elif [ -f "${SOURCE}/${line}" ]
    then # It is a file
        echo "File ${line}"
        scp $SOURCE/$line $HOST:~/$DESTINATION/$line
    else # It does not exist
        echo "${line} is not valid";
        # exit 1
    fi
done <$FILESFILE

#scp ../Python\ Scripts/analyser.py abel:~/FrictionProject/friction/
