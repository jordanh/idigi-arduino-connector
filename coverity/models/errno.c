int * __errno_location (void)
{
    static int errno_value;
    
    return &errno_value;
}

