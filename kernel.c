//MAX_ROWS 25
//MAX_COLS 80

int g_offset = 0;

void print(char *string);

#define VGA_CTRL_REGISTER 0x3d4
#define VGA_DATA_REGISTER 0x3d5
#define VGA_OFFSET_LOW 0x0f
#define VGA_OFFSET_HIGH 0x0e


unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}



void set_cursor(int offset) {
    offset /= 2;
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset >> 8));
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset & 0xff));
}

int get_cursor() {
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    int offset = port_byte_in(VGA_DATA_REGISTER) << 8;
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    offset += port_byte_in(VGA_DATA_REGISTER);
    return offset * 2;
}


void memory_copy(char *source, char *dest, int nbytes){
    int i = 0;
    while (nbytes != 0){
        *(dest + i) = *(source + i);
        i++;
        nbytes--;
    }
}

int length(char *string){
    int i = 0;
    while(*string != 0){
        *string++;
        i++;
    }

    return i;
}

void reverse(char *string){
    int first = 0;
    int last = length(string) - 1;
    char temp;

    while(first < last){
        temp = string[first];
        string[first] = string[last];
        string[last] = temp;

        first++;
        last--;
    }
}


void print_char(char character, int offset) {
    unsigned char *vidmem = (unsigned char *) 0xb8000;
    vidmem[offset] = character;
    vidmem[offset + 1] = 15;
}

void print(char *string){
    while(*string != '\0'){
        if (g_offset >= 80 * 25 * 2){
            memory_copy((char *) (0xb8000 + (80 * 2)), (char *) 0xb8000, 80 * (25 - 1) * 2);
            
            for(int i = 0; i < 80 * 2; i+=2){
                print_char(' ', i + (24 * 160));
            }

            g_offset = g_offset - 2 * 80;
        }

        if (*string == '\n'){
            g_offset = 2 * (((g_offset / (2 * 80)) + 1) * 80 + 0);
            *string++;
            continue;
        }

        print_char(*string++, g_offset);
        g_offset += 2;
    }

    set_cursor(g_offset);
}

void print_int(long number){
    char buffer[1024];

    int a = number/10;
    int b = number;
    int i = 0;

    while (1){
        if (a < 1){
            buffer[i] = b + '0';
            i++;
            break;
        }

        int c = b - (a * 10);
        b = a;
        a = a/10;

        buffer[i] = c + '0';
        i++;
    }

    buffer[i] = '\0';

    reverse(buffer);

    print(buffer);
}

void clear(){
    for(int j = 0; j < 25; j++){
        for(int i = 0; i < 80 * 2; i+=2){
            print_char(' ', i + (j * 160));
        }
    }

    set_cursor(0);
}

void main() {
    clear();
}
