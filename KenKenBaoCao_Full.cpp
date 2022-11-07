#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_INDIVIDUAL 110
#define MAX_LENGTH 81
#define MAX_NUMDIGITS 9
#define INF 999999

int NumDigits = 9;

// Cấu trúc tọa độ
typedef struct
{
    int x, y;
} Coord;

// Chuyển từ tọa độ ô sang một số
int indexOf(Coord c)
{
    return (c.x * NumDigits + c.y);
}

// Chuyển từ tọa độ ô sang một số
Coord coordOf(int vertex)
{
    Coord coord;
    coord.x = vertex / NumDigits;
    coord.y = vertex % NumDigits;
    return coord;
}

// Cấu trúc ràng buộc
typedef struct
{
    int data[MAX_NUMDIGITS * MAX_NUMDIGITS][MAX_NUMDIGITS * MAX_NUMDIGITS];
    int n; // số ô , không gian bài toán
} Constrains;

// Khởi tạo ràng buộc
void initConstrains(Constrains *c, int n)
{
    int i, j;
    for (i = 0; i < n * n; i++)
    {
        for (j = 0; j < n * n; j++)
        {
            c->data[i][j] = 0;
        }
    }
    c->n = n * n;
}

// Thêm ràng buộc
void addConstrains(Constrains *c, Coord source, Coord target)
{
    int u = indexOf(source);
    int v = indexOf(target);
    c->data[u][v] = 1;
}

// Lan truyền ràng buộc
void spreadConstrains(Coord coord, Constrains *c)
{
    int i, j;
    // Tạo ràng buộc cho dòng
    for (j = 0; j < NumDigits; j++)
    {
        if (j != coord.y)
        {
            Coord target = {coord.x, j};
            addConstrains(c, coord, target);
        }
    }
    // Tạo ràng buộc cho cột
    for (i = 0; i < NumDigits; i++)
    {
        if (i != coord.x)
        {
            Coord target = {i, coord.y};
            addConstrains(c, coord, target);
        }
    }
}

// Cấu trúc hộp, khối
typedef struct
{
    int size;
    Coord *coord;
    char op;
    int value;
} Box;

// Cấu trúc danh sách hộp, khối
typedef struct
{
    int size;
    Box B[MAX_LENGTH];
} BoxList;

// Cấu trúc Kenken
typedef struct
{
    int CoordBoxMapping[MAX_NUMDIGITS][MAX_NUMDIGITS]; // Lưu trữ phân vùng cho hộp,khối
    int Result[MAX_NUMDIGITS][MAX_NUMDIGITS];          // Lưu trữ kết quả của kenken
    BoxList ListBoxes;                                 // Danh sách hộp, khối
    Constrains C;                                      // Tập ràng buộc
} KenKen;

// Khởi tạo bảng kenken
void init_KenKen(KenKen *KK)
{
    int i, j;
    initConstrains(&KK->C, NumDigits);
    for (i = 0; i < NumDigits; i++)
    {
        for (j = 0; j < NumDigits; j++)
        {
            KK->Result[i][j] = 0;
            Coord C = {i, j};
            spreadConstrains(C, &KK->C);
        }
    }
}

//Đọc dữ liệu , input cho kenken
/* Ex:
4               --> NumDigits: kích thước Kenken
9               --> KK->ListBoxes.size: số lượng hộp, khối
2 0 0 1 0 / 2   --> Lần lượt:
2 0 1 1 1 + 3           - KK->ListBoxes.B[i].size: số lượng ô của khối
2 0 2 1 2 x 12          - Tọa độ của các ô trong khối tương ứng
1 0 3 = 3               - Toán tử ( = chỉ có 1 ô)
2 1 3 2 3 / 2           - Giá trị của ô
2 2 1 2 2 - 3
2 2 0 3 0 + 4
1 3 1 = 3
2 3 2 3 3 - 2
*/
void readKenKen(KenKen *KK, char *input)
{
    {
        FILE *f = fopen(input, "r");
        // Đọc kích thước kenken và số khối
        fscanf(f, "%d%d", &NumDigits, &KK->ListBoxes.size);
        int i;
        for (i = 0; i < KK->ListBoxes.size; i++)
        {
            // Đọc số lượng ô của khối
            fscanf(f, "%d\n", &KK->ListBoxes.B[i].size);
            KK->ListBoxes.B[i].coord = (Coord *)malloc(sizeof(Coord) * KK->ListBoxes.B[i].size);
            int j;
            for (j = 0; j < KK->ListBoxes.B[i].size; j++)
            {
                // Đọc tọa độ của các ô trong khối tương ứng
                fscanf(f, "%d%d\n", &KK->ListBoxes.B[i].coord[j].x, &KK->ListBoxes.B[i].coord[j].y);
                // Ghi chỉ số khối của ô đang đọc vào Box Map
                KK->CoordBoxMapping[KK->ListBoxes.B[i].coord[j].x][KK->ListBoxes.B[i].coord[j].y] = i;
            }
            // Đọc phép toán của khối
            KK->ListBoxes.B[i].op = fgetc(f);
            // Đọc giá trị kết quả của khối
            fscanf(f, "%d", &KK->ListBoxes.B[i].value);
        }
        init_KenKen(KK);
        fclose(f);
    }
}

// Cấu trúc danh sách
typedef struct
{
    int E[MAX_NUMDIGITS + 1];
    int size;
} List;

// Khởi tạo danh sách
void init_List(List *L)
{
    L->size = 0;
}
// Thêm một phần tử vào cuối danh sách
void append_List(List *L, int x)
{
    L->E[L->size++] = x;
}
// Kiểm tra Kenken có được điền đầy hay không
int isFilledKenKen(KenKen KK)
{
    int i, j;
    for (i = 0; i < NumDigits; i++)
    {
        for (j = 0; j < NumDigits; j++)
        {
            if (KK.Result[i][j] == 0)
                return 0;
        }
    }
    return 1;
}

// Tìm miền giá trị của một ô
List getAvailableValues(Coord coord, KenKen KK)
{
    int i;
    int values[NumDigits]; // Chỉ sử dụng từ 1 đến NumDigits (các giá trị có thể điền cho một ô)
    char op = KK.ListBoxes.B[KK.CoordBoxMapping[coord.x][coord.y]].op;
    int result = KK.ListBoxes.B[KK.CoordBoxMapping[coord.x][coord.y]].value;
    int sizeOfBox = KK.ListBoxes.B[KK.CoordBoxMapping[coord.x][coord.y]].size;

    for (i = 1; i <= NumDigits; i++)
        values[i] = 0;
    if (op == '=')
        values[result] = 1;
    else if (op == '/' && sizeOfBox == 2)
    {
        values[1] = 1;
        values[result] = 1;
        int j;
        for (i = NumDigits; i > (NumDigits) / 2; i--)
        {
            for (j = (NumDigits / 2); j >= 1; j--)
            {
                if (i / j == result && i % j == 0)
                {
                    values[i] = 1;
                    values[j] = 1;
                }
            }
        }
    }
    else if (op == '-' && sizeOfBox == 2)
    {
        for (i = 1; i <= NumDigits; i++)
        {
            if (i + result > NumDigits)
                break;
            values[i] = 1;
            values[i + result] = 1;
        }
    }
    else if (op == '+')
    {
        if (sizeOfBox == 2)
        {
            int j;
            for (i = 1; i <= NumDigits; i++)
            {
                for (j = 1; j <= NumDigits; j++)
                {
                    if (i != j && i + j == result)
                    {
                        values[i] = 1;
                        values[j] = 1;
                    }
                }
            }
        }
        else
        {
            for (i = 1; i <= NumDigits; i++)
                values[i] = 1;
        }
    }
    else if (op == 'x')
    {
        if (sizeOfBox == 2)
        {
            int i, j;
            for (i = 1; i <= NumDigits; i++)
            {
                for (j = 1; j <= NumDigits; j++)
                {
                    if (i != j && i * j == result)
                    {
                        values[i] = 1;
                        values[j] = 1;
                    }
                }
            }
        }
        else
        {
            for (i = 1; i <= NumDigits; i++)
                values[i] = 1;
        }
    }
    int vertex = indexOf(coord); // Lấy tọa độ ô cần tính miền giá trị chuyển sang chỉ số đỉnh trong ma trận rành buộc
    for (i = 0; i < KK.C.n; i++)
    {
        if (KK.C.data[vertex][i] == 1) // Nếu có 2 đỉnh tồn tại ràng buộc
        {
            Coord temp = coordOf(i); // Chuyển về dạng tọa độ
            values[KK.Result[temp.x][temp.y]] = 0; // Loại bỏ giá trị đã có ra khỏi miền giá trị
        }
    }
    // Lấy những giá trị hợp lệ có thể điền vào
    List L;
    init_List(&L);
    for (i = 1; i <= NumDigits; i++)
    {
        if (values[i] == 1)
        {
            append_List(&L, i);
        }
    }
    return L;
}
// Tìm ô có miền giá trị nhỏ nhất
Coord getNextMinDomainCell(KenKen KK)
{
    int min_value = INF;
    int i, j;
    Coord MinCell = {0, 0};
    for (i = 0; i < NumDigits; i++)
    {
        for (j = 0; j < NumDigits; j++)
        {
            if (KK.Result[i][j] == 0) // Chỉ xét những ô chưa được điền số
            {
                Coord C = {i, j};

                int length = getAvailableValues(C, KK).size;
                if (length < min_value)
                {
                    MinCell = C;
                    min_value = length;
                }
            }
        }
    }
    return MinCell;
}
// Kiểm tra một khối có được điền đầy hay không
int isFilledBox(int Box_index, KenKen KK)
{
    int i;
    for (i = 0; i < KK.ListBoxes.B[Box_index].size; i++)
    {
        Coord C = KK.ListBoxes.B[Box_index].coord[i];
        if (KK.Result[C.x][C.y] == 0)
            return 0;
    }
    return 1;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}
// Kiểm tra kết quả của khối
int checkValidBox(int Box_index, KenKen KK)
{
    if (isFilledBox(Box_index, KK))
    {
        char op = KK.ListBoxes.B[Box_index].op;
        int Con_Value = KK.ListBoxes.B[Box_index].value;
        int S;
        if (op == 'x' || op == '/')
            S = 1;
        if (op == '-' || op == '+' || op == '=')
            S = 0;
        int i;
        for (i = 0; i < KK.ListBoxes.B[Box_index].size; i++)
        {
            Coord C = KK.ListBoxes.B[Box_index].coord[i];
            if (op == 'x')
                S *= KK.Result[C.x][C.y];
            if (op == '/')
                S = max(S, KK.Result[C.x][C.y]) / min(S, KK.Result[C.x][C.y]);
            if (op == '=')
                S = KK.Result[C.x][C.y];
            if (op == '+')
                S += KK.Result[C.x][C.y];
            if (op == '-')
                S = abs(S - KK.Result[C.x][C.y]);
        }
        if (S == Con_Value)
            return 1;
        else
            return 0;
    }
    return 1;
}
// Hiển thị kết quả của kenken
void printKenKen(KenKen KK)
{
    int i, j;
    for (i = 0; i < NumDigits; i++)
    {
        printf("\n");
        for (j = 0; j < NumDigits; j++)
        {
            printf("%d ", KK.Result[i][j]);
        }
    }
    printf("\n");
}

long long exploredCounter = 0; // Đếm số trạng thái
// Thuật toán quay lui cho bài toán thỏa mãn ràng buộc Kenken
int KKBackTracking(KenKen *KK, KenKen *Backup)
{
    // Kiểm tra kết quả Kenken đã đúng chưa
    if (isFilledKenKen(*KK))
        return 1;
    Coord coord = getNextMinDomainCell(*KK); // Chọn ô cần điền
    List L = getAvailableValues(coord, *KK); // Lấy miền giá trị hợp lệ cho ô cần điền
    if (L.size == 0)
    {
        return 0; // Nếu không có giá trị để điền, trả về 0
    }
    int i;
    for (i = 0; i <L.size; i++)
    {
        KK->Result[coord.x][coord.y] = L.E[i];
        exploredCounter++;
        if (exploredCounter > 500) // Giới hạn số trạng thái chạy không vượt hơn 500
            throw exploredCounter;
        int boxIndex = KK->CoordBoxMapping[coord.x][coord.y]; // Lấy chỉ số khối chứa ô đang điền
        if (checkValidBox(boxIndex, *KK)) // Kiểm tra kết quả phép toán trong khôid chứa ô đang điền
        {
            // Nếu hợp lệ thì gọi đệ quy điền ô tiếp theo
            if (KKBackTracking(KK, Backup))
            {
                return 1;
            }
        }
        KK->Result[coord.x][coord.y] = 0; // Xóa bỏ giá trị điền sai
        *Backup = *KK; // Lưu lại Kenken trong sau mỗi lần quay lui
    }
    return 0;
}
// Lấy giá trị số nguyên ngẫu nhiên
int int_rand(int from, int to)
{
    int i;
    int num = (rand() % (to - from + 1)) + from;
    return num;
}
// Lấy giá trị số thực ngẫu nhiên
float float_rand(float min, float max)
{
    float scale = rand() / (float)RAND_MAX;
    return min + scale * (max - min);
}
// Giải quyết bài toán thỏa mãn ràng buộc cho Kenken
int solve_KenKen_with_constraints(KenKen *KK, KenKen *Backup)
{
    *Backup = *KK;
    try
    {
        if (KKBackTracking(KK, Backup))
        {
            printf("\nCAN BE SOLVED WITH CONSTRAINTS ALGORITHM. EXPLORED %d STATES\n", exploredCounter);
            return 1;
        }
    }
    catch (long long explored)
    {
        printf("\nCANNOT BE SOLVED WITH CONSTRAINT ALGORITHM\n");
    }
    int i, j;
    //Xử lý Backup để sử dụng cho giải thuật giải di truyền.
    for (i = 0; i < Backup->ListBoxes.size; i++)
    {
        if (!isFilledBox(i, *Backup))
        {

            for (j = 0; j < Backup->ListBoxes.B[i].size; j++)
            {
                Coord C = Backup->ListBoxes.B[i].coord[j];
                Backup->Result[C.x][C.y] = 0;
            }
        }
    }
     return 0;
}
// Cấu trúc 1 cá thể
typedef struct
{
    int Chromosome[MAX_NUMDIGITS][MAX_NUMDIGITS];
    double Fitness;
} Individual;
// Khởi tạo cá thể 
void initIndividual(Individual *ind)
{
    int i, j;
    for (i = 0; i < NumDigits; i++)
    {
        for (j = 0; j < NumDigits; j++)
        {
            ind->Chromosome[i][j] = 0;
        }
    }
    ind->Fitness = 0;
}
// Cấu trúc 1 quần thể
typedef struct
{
    Individual Inds[MAX_INDIVIDUAL];
    int size;
} Population;
// Khởi tạo quần thể
void initPopulation(Population *P)
{
    P->size = 0;
}
// Thêm cá thể vào quần thể
void appendPopulation(Population *P, Individual ind)
{
    P->Inds[P->size++] = ind;
}
// Hiển thị cá thể
void print_individual(Individual i)
{
    printf("\n");
    for (int a = 0; a < NumDigits; a++)
    {
        for (int b = 0; b < NumDigits; b++)
        {
            printf("%d ", i.Chromosome[a][b]);
        }
        printf("\n");
    }
}
// Tìm miền giá trị cho quá trình tạo quần thể
List avalableValuesInd(int row, int col, Individual ind, KenKen KK)
{
    int values[NumDigits];
    int i;
    for (i = 1; i <= NumDigits; i++)
        values[i] = 1;
    for (i = 0; i < NumDigits; i++)
    {
        if (col != i)
            values[ind.Chromosome[row][i]] = 0; // Loại bỏ những giá trị đã có trên cùng dòng trong cá thể
        if (KK.Result[i][col] != 0 && row != i)
            values[KK.Result[i][col]] = 0; // Loại bỏ những giá trị đã có trên cùng cột trong backup
        if (KK.Result[row][i] != 0 && col != i)
            values[KK.Result[row][i]] = 0; // Loại bỏ những giá trị đã có trên cùng dòng trong backup
    }
    List L;
    init_List(&L);

    for (i = 1; i < NumDigits + 1; i++)
    {
        if (values[i] == 1)
            append_List(&L, i);
    }
    return L;
}
// Sắp xếp vị trí các cá thể dựa trên fitness
void sortPopulation(Population *P)
{
    int i, j;
    for (i = 0; i < P->size - 1; i++)
    {
        for (j = i + 1; j < P->size; j++)
        {
            if (P->Inds[i].Fitness < P->Inds[j].Fitness)
            {
                Individual tmp = P->Inds[i];
                P->Inds[i] = P->Inds[j];
                P->Inds[j] = tmp;
            }
        }
    }
}

void updateFitness(Individual *ind, KenKen KK)
{
    // Bước 1: Tính BoxF
    int i, j;
    double boxsum = 0.0;
    // Tính số box có phép toán đúng
    for (i = 0; i < KK.ListBoxes.size; i++)
    {
        char Operator = KK.ListBoxes.B[i].op;
        int Con_Value = KK.ListBoxes.B[i].value;
        int S = 0;
        if (Operator == 'x' || Operator == '/')
            S = 1;
        if (Operator == '-' || Operator == '+' || Operator == '=')
            S = 0;
        int j;
        for (j = 0; j < KK.ListBoxes.B[i].size; j++)
        {
            Coord C = KK.ListBoxes.B[i].coord[j];
            if (Operator == 'x')
                S *= ind->Chromosome[C.x][C.y];
            if (Operator == '/')
                S = max(S, ind->Chromosome[C.x][C.y]) / min(S, ind->Chromosome[C.x][C.y]);
            if (Operator == '=')
                S = ind->Chromosome[C.x][C.y];
            if (Operator == '+')
                S += ind->Chromosome[C.x][C.y];
            if (Operator == '-')
                S = abs(S - ind->Chromosome[C.x][C.y]);
        }
        if (S == Con_Value)
            boxsum = boxsum + 1.0;
    }
    boxsum = boxsum / KK.ListBoxes.size; // Tỷ lệ boxF có kết quả phép toán đúng
    // Bước 2: Tính colF
    int columncount[NumDigits]; //Lưu tần số xuất hiện của một con số i
    for (i = 1; i <= NumDigits; i++)//
        columncount[i] = 0;
    double columnsum = 0.0;
    // Lấy số cột không bị trùng số trong cá thể
    for (i = 0; i < NumDigits; i++)
    {
        // Cập nhật tần số xuất hiện của một con số
        for (j = 0; j < NumDigits; j++)
        {
            columncount[ind->Chromosome[j][i]] += 1;//
        }
        //Đếm số lượng con số có tần số xuất hiện lớn hơn 0
        int nonzero = 0;
        for (j = 1; j <= NumDigits; j++)
        {
            if (columncount[j] != 0)
                nonzero += 1;
        }
        columnsum = columnsum + ((1.0 * nonzero) / (1.0 * NumDigits));
        for (j = 1; j <= NumDigits; j++)
            columncount[j] = 0;
    }
    columnsum = columnsum / NumDigits;
    //Bước 3: Tính Fittness.
    if (((int)boxsum == 1) && ((int)columnsum) == 1)
    {
        ind->Fitness = 1.0;
    }
    else
    {
        ind->Fitness = (boxsum) * (columnsum);
    }
}
// Khởi tạo giá trị cho quần thể
void seedPopulation(Population *P, int indNum, KenKen KK)
{
    initPopulation(P);
    int p;
    for (p = 0; p < indNum; p++)
    {
        Individual ind;
        initIndividual(&ind);
        int isValid = 1; // Biểu thị việc tạo cá thể thành công
        int i, j;
        for (i = 0; i < NumDigits; i++)
        {
            for (j = 0; j < NumDigits; j++)
            {
                // Lấy giá trị đã có sẵn từ backup nếu có
                if (KK.Result[i][j] != 0)
                    ind.Chromosome[i][j] = KK.Result[i][j];
                else
                {
                    List availables = avalableValuesInd(i, j, ind, KK);
                    if (availables.size == 0)
                    {
                        isValid = 0;
                        break;
                    }
                    else
                    {
                        // Lấy ngẫu nhiên miền giá trị điền vào ô
                        int randomIndex = int_rand(0, availables.size - 1);
                        ind.Chromosome[i][j] = availables.E[randomIndex];
                    }
                }
            }
            // Nếu tạo cá thể thất bại, tạo lại cá thể ấy
            if (isValid == 0)
            {
                p--;
                break;
            }
        }
        // Nếu tạo thành công thì tính fitness cho cá thể và thêm cá thể vào quần thể
        if (isValid == 1)
        {
            updateFitness(&ind, KK);
            appendPopulation(P, ind);
        }
    }
    sortPopulation(P); // Sắp xếp lại quần thể theo fitness
}
// Chọn lọc cá thể từ quần thể 
Individual compete(Population P, double selectionRate)
{
    //Lấy ngẫu nhiên 2 cá thể
    Individual ind1 = P.Inds[int_rand(0, P.size - 1)];
    Individual ind2 = P.Inds[int_rand(0, P.size - 1)];
    Individual betterInd, worseInd;
    if (ind1.Fitness > ind2.Fitness)
    {
        betterInd = ind1;
        worseInd = ind2;
    }
    else
    {
        betterInd = ind2;
        worseInd = ind1;
    }
    double ran = float_rand(0.0, 1.0); // Xác suất chọn 1 trong 2 cá thể vừa chọn
    if (ran <= selectionRate) // selectionRate: tỷ lệ chọn lọc
        return betterInd;
    return worseInd;
}
// Kiểm tra có tồn tại giá trị bị trùng trên cùng 1 cột
int isValidCol(int val, int colIndex, KenKen KK)
{
    int row;
    for (row = 0; row < NumDigits; row++)
    {
        if (KK.Result[row][colIndex] == val)
            return 0;
    }
    return 1;
}
// Đột biến cá thể
void mutate(double mutationRate, Individual *Ind, KenKen KK)
{
    double r = float_rand(0, 1.0);
    if (r < mutationRate) //mutationRate: tỷ lệ xảy ra đột biến
    {
        for (int i = 0; i < 2; i++)
        {
            int g = int_rand(0, NumDigits - 1); // Lấy chỉ số dòng
            // Lấy chỉ số ở 2 cột cần hoán vị
            int x = int_rand(0, NumDigits - 1);
            int y = int_rand(0, NumDigits - 1);
            // Nếu việc hoán vị không gây ra sự trùng lặp trên cùng 1 cột thì tiến hành đột biến
            if (isValidCol(Ind->Chromosome[g][y], x, KK) && isValidCol(Ind->Chromosome[g][x], y, KK))
            {
                int t = Ind->Chromosome[g][x];
                Ind->Chromosome[g][x] = Ind->Chromosome[g][y];
                Ind->Chromosome[g][y] = t;
            }
        }
    }
}
// Lai ghép 2 cá thể
Population crossover(Individual Parent1, Individual Parent2, double crossoverRate, KenKen KK)
{
    Population population;
    initPopulation(&population);
    Individual chidren1 = Parent1;
    Individual chidren2 = Parent2;
    double r = float_rand(0, 1.0);
    if (r < crossoverRate) // crossoverRate: tỷ lệ xảy ra lai ghép
    {
        int soluong = int_rand(1, NumDigits); // Lấy ngẫu nhiên số lượng dòng trong cá thể để lai
        for (int k = 0; k < soluong; k++)
        {
            int row = int_rand(0, NumDigits - 1); // Lấy ngẫu nhiên dòng lai
            for (int i = 0; i < NumDigits; i++)
            {
                int t = chidren1.Chromosome[row][i];
                chidren1.Chromosome[row][i] = chidren2.Chromosome[row][i];
                chidren2.Chromosome[row][i] = t;
            }
        }
    }
    appendPopulation(&population, chidren1);
    appendPopulation(&population, chidren2);
    return population;
}
Individual best_indi;
// Thuật toán di truyền giải quyết bài toán KenKen
Individual solve_KenKen(KenKen KK)
{
    // Bước 1: Khởi tạo các biến.
    int Num_Candidates = 100;               // Số lượng cá thể trong quần thể.
    int Num_Elites = 0.05 * Num_Candidates; // Số lượng cá thể ưu tú trong quần thể.
    int Num_Generations = 15000;            // Số thế hệ.
    double mutationRate = 0.06;
    // Bước 2: Khởi tạo quần thể ban đầu.
    Population P;
    initPopulation(&P);
    seedPopulation(&P, Num_Candidates, KK);

    // Bước 3: Thực hiện quá trình di truyền.
    int i, state = 0;
    double best_Fitness = 0.0;
    for (i = 0; i < Num_Generations; i++)
    {
        // Kiểm tra quần thể hiện tại (kiểm tra xem đã tìm được đáp án chưa).
        sortPopulation(&P);
        if (P.Inds[0].Fitness == 1)
            return P.Inds[0];
        if (best_indi.Fitness < P.Inds[0].Fitness)
        {
            best_indi = P.Inds[0];
        }
        // Khởi tạo một quần thể.
        Population Next_P;
        initPopulation(&Next_P);
        Population Elites;
        initPopulation(&Elites); // Lấy các cá thể ưu tú để giữ lại cho thế hệ tiếp theo.
        int k;
        for (k = 0; k < Num_Elites; k++)
            appendPopulation(&Elites, P.Inds[k]);
        int count;
        for (count = Num_Elites; count < Num_Candidates; count += 2) // Thực hiện chọn lọc tự nhiên, lai ghép và đột biến để tạo nên các cá thể mới cho thế hệ tiếp theo.
        {
            // Lấy hai cá thể cha mẹ thông qua chọn lọc tự nhiên.
            Individual Parent1 = compete(P, 0.85);
            Individual Parent2 = compete(P, 0.85);
            Population Result;
            initPopulation(&Result);
            // Thực hiện lai ghép để tạo ra hai cá thể con mới.
            Result = crossover(Parent1, Parent2, 0.9, KK);
            //Đột biến con thứ nhất.
            mutate(mutationRate, &Result.Inds[0], KK);
            updateFitness(&Result.Inds[0], KK);
            //Đột biến con thứ hai.
            mutate(mutationRate, &Result.Inds[1], KK);
            updateFitness(&Result.Inds[1], KK);
            //Thêm 2 cá thể vào quần thể
            appendPopulation(&Next_P, Result.Inds[0]);
            appendPopulation(&Next_P, Result.Inds[1]);
        }
        // Tạo quần thể cho thế hệ tiếp theo.
        for (k = 0; k < Elites.size; k++)
            appendPopulation(&Next_P, Elites.Inds[k]);
        P = Next_P;
        // Cập nhật hàm thích nghi cho mọi cá thể và sắp xếp quần thể mới vừa tạo.
        for (k = 0; k < P.size; k++)
            updateFitness(&P.Inds[k], KK);
        sortPopulation(&P);

        // Kiểm tra quần thể mới vừa tạo (kiểm tra xem có bị bế tắc từ 100 lần trở lên hay không).
        if (P.Inds[0].Fitness != P.Inds[1].Fitness)
            state = 0;
        else
            state += 1;
        if (state >= 100) // Nếu bị bế tắc, khởi tạo lại quần thể từ ban đầu.
        {
            seedPopulation(&P, Num_Candidates, KK);

            mutationRate = 0.06;
        }
    }
    return P.Inds[0];
}

int main()
{
    time_t start, end;
    srand(time(0));
    KenKen KK;
    readKenKen(&KK, "inputs/input7.txt");
    KenKen Backup;
    if(solve_KenKen_with_constraints(&KK, &Backup))
    {
        printKenKen(KK);
    }
    else
    {
        start = clock();

        printKenKen(Backup);
        Individual KQ = solve_KenKen(Backup);
        print_individual(KQ);
        end = clock();
        float duration = (float)(end - start) / CLOCKS_PER_SEC;
        printf("\nTimes Taken: %.4fsec", duration);
        print_individual(best_indi);
        printf("\n %.2f", best_indi.Fitness);
    }

    return 0;
}