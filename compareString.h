//MUST terminate string with semicolon unlesss the strings are the same size and have been cleared
bool strEquals(char str1[32], char str2[32]){
    int i = 0;
    while(i < 32 && str1[i] != ';' && str2[i] != ';' && str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0'){
        i++;
    }
    if(i == 32 || str1[i] != str2[i]){
        return false;
    }else{
        return true;
    }
}