echo "echo Restoring environment" > "/home/half/chatterino2/deactivate_conanbuildenv-release-x86_64.sh"
for v in CXX CC
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "/home/half/chatterino2/deactivate_conanbuildenv-release-x86_64.sh"
    else
        echo unset $v >> "/home/half/chatterino2/deactivate_conanbuildenv-release-x86_64.sh"
    fi
done


export CXX="/usr/bin/g++"
export CC="/usr/bin/gcc"