unsigned short toUInt16(unsigned char a, unsigned char b) {
    unsigned short shortA = ((unsigned short)a); //LSB
    unsigned short shortB = ((unsigned short)b)<<8; //MSB
    //printf("\t\t\ttoUInt16: %#1x %#1x\n", shortA, shortB); //debug
    return shortA|shortB;
}

unsigned int toUInt32(unsigned short a, unsigned short b) {
    unsigned int intA = ((unsigned int)a); //LSBs
    unsigned int intB = ((unsigned int)b)<<8; //MSBs
    //printf("\t\ttoUInt32: %#1x %#1x\n", intA, intB); //debug
    return intA|intB;
}

unsigned int bytesToUInt(unsigned char *arr) {
    unsigned char charA = (unsigned char)*arr;
    unsigned char charB = (unsigned char)*(arr+1);
    unsigned char charC = (unsigned char)*(arr+2);
    unsigned char charD = (unsigned char)*(arr+3);
    //printf("\t\tbytesToUInt: %#1x %#1x %#1x %#1x\n", charA, charB, charC, charD); //debug
    unsigned short shortA = toUInt16(charA, charB);
    unsigned short shortB = toUInt16(charC, charD);
    return toUInt32(shortA, shortB);
}

void uIntToBytes(unsigned int num, unsigned char *bytes) {
    //(unsigned int*)bytes = num; //if you want to work on little endian systems only.
    bytes[0] = (unsigned char)(num & 0xFF);     //low-order LSB
    bytes[1] = (unsigned char)((num>>8) & 0xFF);//high-order LSB
    bytes[2] = (unsigned char)((num>>16) & 0xFF);//low-order  MSB
    bytes[3] = (unsigned char)((num>>24) & 0xFF);//high-order MSB
}

void uShortToBytes(unsigned short num, unsigned char *bytes) {
    //(unsigned short*)bytes = num; //if you want to work on little endian systems only.
    bytes[0] = (unsigned char)(num & 0xFF);     //low-order LSB
    bytes[1] = (unsigned char)((num>>8) & 0xFF);//high-order LSB
}
