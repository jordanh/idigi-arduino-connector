BEGIN {sum=0}
{
    if ($0 ~ /^.(text|bss|data) /) {
        print $1, $3
        sum = sum + $3
    }
}
END {print sum}

