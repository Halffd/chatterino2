echo "echo Restoring environment" > "/home/half/chatterino2/deactivate_conanrunenv-release-x86_64.sh"
for v in PATH LD_LIBRARY_PATH DYLD_LIBRARY_PATH
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "/home/half/chatterino2/deactivate_conanrunenv-release-x86_64.sh"
    else
        echo unset $v >> "/home/half/chatterino2/deactivate_conanrunenv-release-x86_64.sh"
    fi
done


export PATH="/home/half/.conan2/p/opense2843fb130fa2/p/bin:$PATH"
export LD_LIBRARY_PATH="/home/half/.conan2/p/opense2843fb130fa2/p/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="/home/half/.conan2/p/opense2843fb130fa2/p/lib:$DYLD_LIBRARY_PATH"