#include "witcher3_ultrawide_patcher.h"
using namespace std;
namespace fs = filesystem;

const vector<unsigned char> TARGET = {0x39, 0x8E, 0xE3, 0x3F};
const vector<unsigned char> RES_2560X1080 = {0x26, 0xB4, 0x17, 0x40};
const vector<unsigned char> RES_3440X1440 = {0x8E, 0xE3, 0x18, 0x40};
const vector<unsigned char> RES_3840X1600 = {0x9A, 0x99, 0x19, 0x40};
const vector<unsigned char> RES_5120X1440 = {0x39, 0x8E, 0x63, 0x40};
const vector<unsigned char> RES_5120X2160 = {0x26, 0xB4, 0x17, 0x40};
const vector<unsigned char> RES_6880X2880 = {0x8E, 0xE3, 0x18, 0x40};

void backup(string filepath)
{
    fs::copy_file(filepath, filepath + ".bak");
}

void print_options()
{
    string resolution[6] = {"2560x1080", "3440x1440", "3840x1600", "5120x1440", "5120x2160", "6880x2880"};
    cout << "Select target resolution:" << endl;
    for (int i = 0; i < size(resolution); i++)
    {
        cout << i + 1 << ": " << resolution[i] << endl;
    }
}

const vector<unsigned char> get_resolution_from_option()
{
    while (1)
    {
        int option;
        cin >> option;

        switch (option)
        {
        case 1:
            cout << "2560x1080 selected..." << endl;
            return RES_2560X1080;
        case 2:
            cout << "3440x1440 selected..." << endl;
            return RES_3440X1440;
        case 3:
            cout << "3840x1600 selected..." << endl;
            return RES_3840X1600;
        case 4:
            cout << "5120x1440 selected..." << endl;
            return RES_5120X1440;
        case 5:
            cout << "5120x2160 selected..." << endl;
            return RES_5120X2160;
        case 6:
            cout << "6880x2880 selected..." << endl;
            return RES_6880X2880;
        default:
            cout << "Choose between 1-6:" << endl;
        }
    }
}

void print_unsigned_array(const vector<unsigned char> array)
{
    for (int i = 0; i < 4; i++)
    {
        cout << hex << "0x" << setw(2) << setfill('0') << uppercase << +array[i] << " ";
    }
}

vector<int> find_offsets(fstream &input)
{
    vector<int> offsets{};
    vector<unsigned char> buffer;
    copy(
        istreambuf_iterator<char>(input),
        istreambuf_iterator<char>(),
        back_inserter(buffer));

    vector<unsigned char>::iterator begin = buffer.begin();
    while ((begin = search(begin, buffer.end(), TARGET.begin(), TARGET.end())) != buffer.end())
    {
        int offset = distance(buffer.begin(), begin);
        offsets.push_back(offset);
        cout << "Found index: " << offset << endl;
        begin++;
    }

    return offsets;
}

void writeToStream(fstream &input, int offset, vector<unsigned char> data)
{
    input.seekg(offset);
    char output[4];
    input.read(output, 0x04);
    cout << "Replacing " << hex << "0x" << +static_cast<unsigned char>(output[0]) << " 0x" << +static_cast<unsigned char>(output[1]) << " 0x" << +static_cast<unsigned char>(output[2]) << " 0x" << +static_cast<unsigned char>(output[3]) << " at 0x" << offset << endl;

    cout << "Patching bytes to file..." << endl;
    input.seekg(offset);
    input.write((const char *)&data[0], 0x04);
}

int main(int argc, char *argv[])
{
    string filepath = argv[1];

    cout << "Selected file: " << filepath << endl;
    cout << "Backing up original..." << endl;

    backup(filepath);

    cout << "Wrote backup: " << (filepath + ".bak") << endl;

    print_options();

    const vector<unsigned char> target_resolution = get_resolution_from_option();

    cout << "Reading file to stream..." << endl;
    fstream executable(filepath, ios::in | ios::out | ios::binary);
    if (!executable)
    {
        cout << "Cannot open file!" << endl;
        return 1;
    }

    cout << "Looking for target: ";
    print_unsigned_array(TARGET);
    cout << endl;

    cout << "Replacment: ";
    print_unsigned_array(target_resolution);
    cout << endl;

    vector<int> offsets = find_offsets(executable);

    for (int offset : offsets)
    {
        writeToStream(executable, offset, target_resolution);
    }

    cout << "File patched..." << endl;

    executable.close();

    int pause_hack;
    cin >> pause_hack;

    return 0;
}
