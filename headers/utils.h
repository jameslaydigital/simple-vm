unsigned short toUInt16(unsigned char a, unsigned char b) {
    unsigned short shortA = ((unsigned short)a); //LSB
    unsigned short shortB = ((unsigned short)b)<<8; //MSB
    return shortA|shortB;
}

unsigned int toUInt32(unsigned short a, unsigned short b) {
    unsigned int intA = ((unsigned int)a); //LSBs
    unsigned int intB = ((unsigned int)b)<<8; //MSBs
    return intA|intB;
}

unsigned int bytesToUInt(unsigned char *arr) {
    unsigned short shortA = toUInt16(arr[0], arr[1]);
    unsigned short shortB = toUInt16(arr[2], arr[3]);
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
