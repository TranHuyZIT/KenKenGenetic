#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_INDIVIDUAL 1050
#define MAX_LENGTH 81
#define MAX_NUMDIGITS 9
int NumDigits = 9;

//Cấu trúc tọa độ
typedef struct
{
    int x, y;
} Coord;

//Chuyển từ tọa độ ô sang một số
int indexOf(Coord c)
{
    return (c.x * NumDigits + c.y);
}

//Chuyển từ tọa độ ô sang một số
Coord coordOf(int vertex)
{
    Coord coord;
    coord.x = vertex / NumDigits;
    coord.y = vertex % NumDigits;
    return coord;
}

//Cấu trúc ràng buộc
typedef struct
{
    int data[MAX_NUMDIGITS * MAX_NUMDIGITS][MAX_NUMDIGITS * MAX_NUMDIGITS];
    int n; //số ô , không gian bài toán
} Constrains;

//Khởi tạo ràng buộc
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

//Thêm ràng buộc 
void addConstrains(Constrains *c, Coord source, Coord target)
{
    int u = indexOf(source);
    int v = indexOf(target);
    c->data[u][v] = 1;
}

//Lan truyền ràng buộc
void spreadConstrains(Coord coord, Constrains *c)
{
    int i, j;
    //Tạo ràng buộc cho dòng
    for (j = 0; j < NumDigits; j++)
    {
        if (j != coord.y)
        {
            Coord target = {coord.x, j};
            addConstrains(c, coord, target);
        }
    }
    //Tạo ràng buộc cho cột
    for (i = 0; i < NumDigits; i++)
    {
        if (i != coord.x)
        {
            Coord target = {i, coord.y};
            addConstrains(c, coord, target);
        }
    }
}

//Cấu trúc hộp, khối
typedef struct
{
    int size;
    Coord *coord;
    char op;
    int value;
} Box;

//Cấu trúc danh sách hộp, khối
typedef struct
{
    int size;
    Box B[MAX_LENGTH];
} BoxList;

//Cấu trúc Kenken
typedef struct
{
    int CoordBoxMapping[MAX_NUMDIGITS][MAX_NUMDIGITS]; //Lưu trữ phân vùng cho hộp,khối
    int Result[MAX_NUMDIGITS][MAX_NUMDIGITS]; //Lưu trữ kết quả của kenken
    BoxList ListBoxes;  //Danh sách hộp, khối
    Constrains C;   //Tập ràng buộc 
} KenKen;

//Khởi tạo bảng kenken
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
2 0 2 1 2 x 12          - Cặp tọa độ của các ô của khối tương ứng (Xi,Yi)
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
        fscanf(f, "%d%d", &NumDigits, &KK->ListBoxes.size);
        int i;
        for (i = 0; i < KK->ListBoxes.size; i++)
        {
            fscanf(f, "%d\n", &KK->ListBoxes.B[i].size);
            KK->ListBoxes.B[i].coord = (Coord *)malloc(sizeof(Coord) * KK->ListBoxes.B[i].size);
            int j;
            for (j = 0; j < KK->ListBoxes.B[i].size; j++)
            {
                fscanf(f, "%d%d\n", &KK->ListBoxes.B[i].coord[j].x, &KK->ListBoxes.B[i].coord[j].y);
                //Ghi chỉ số khối chứa ô đang xét vào Box Map
                KK->CoordBoxMapping[KK->ListBoxes.B[i].coord[j].x][KK->ListBoxes.B[i].coord[j].y] = i;
            }
            KK->ListBoxes.B[i].op = fgetc(f);
            fscanf(f, "%d", &KK->ListBoxes.B[i].value);
        }
        init_KenKen(KK);
        fclose(f);
    }
}

//Cấu trúc danh sách
typedef struct
{
    int E[MAX_NUMDIGITS + 1];
    int size;
} List;

//Khởi tạo danh sách
List init_List(List *L)
{
    L->size = 0;
}
// Thêm một phần tử vào cuối danh sách
void append_List(List *L, int x)
{
    L->E[L->size++] = x;
}
// Xóa một phần tử trong danh sách
void delete_List(List *L, int x)
{
    int i, j;
    for (i = 0; i < L->size; i++)
    {
        if (L->E[i] == x)
        {
            for (j = i; j < L->size - 1; j++)
            {
                L->E[j] = L->E[j + 1];
            }
        }
    }
    L->size--;
}
// Tìm giá trị 0 trong danh sách
int haveZero(List L)
{
    int i;
    for (i = 0; i < L.size; i++)
    {
        if (L.E[i] == 0)
            return 1;
    }
    return 0;
}
// Tìm một giá trị trong danh sách
int findX(List L, int X)
{
    int i;
    for (i = 0; i < L.size; i++)
    {
        if (L.E[i] == X)
            return i;
    }
    return -1;
}
void print_list(List l) {
    for (int i =0 ; i<l.size; i++) {
        printf("%d ",l.E[i]);
    }
    printf("\n");
}
// Tìm vị trí phần tử giống nhau đầu tiên của 2 danh sách
int findSameElement(List L, List Remaining)
{
    int i, j;
    for (i = 0; i < L.size; i++)
    {
        for (j = 0; j < Remaining.size; j++)
        {
            if (L.E[i] == Remaining.E[j])
            {
                return i;
            }
        }
    }
    return -1;
}

//Kiểm tra Kenken có được điền đầy hay không
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

//Tìm miền giá trị khả dĩ của một ô
List getAvailableValues(Coord coord, KenKen KK)
{
    int i;
    int values[NumDigits + 1]; //Chỉ sử dụng từ 1 đến NumDigits (các giá trị có thể điền cho một ô)
    char op = KK.ListBoxes.B[KK.CoordBoxMapping[coord.x][coord.y]].op;
    int result = KK.ListBoxes.B[KK.CoordBoxMapping[coord.x][coord.y]].value;
    int sizeOfBox = KK.ListBoxes.B[KK.CoordBoxMapping[coord.x][coord.y]].size;

    for (i = 1; i <= NumDigits; i++)
        values[i] = 0;
    if (op == '=')
        values[result] = 1;
    else if (op == '/')
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
            for (i = 1; i <= MAX_NUMDIGITS; i++)
            {
                for (j = 1; j <= MAX_NUMDIGITS; j++)
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
            for (i = 1; i <= MAX_NUMDIGITS; i++)
            {
                for (j = 1; j <= MAX_NUMDIGITS; j++)
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
    int vertex = indexOf(coord);
    for (i = 0; i < KK.C.n; i++)
    {
        if (KK.C.data[vertex][i] == 1)
        {
            Coord temp = coordOf(i);
            values[KK.Result[temp.x][temp.y]] = 0;
        }
    }
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
#define INF 999999
Coord getNextMinDomainCell(KenKen KK)
{
    int min_value = INF;
    int i, j;
    Coord MinCell = {0, 0};
    for (i = 0; i < NumDigits; i++)
    {
        for (j = 0; j < NumDigits; j++)
        {
            if (KK.Result[i][j] == 0)
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

int exploredCounter = 0;
int KKBackTracking(KenKen *KK, KenKen *Backup)
{
    if (isFilledKenKen(*KK))
        return 1;
    Coord coord = getNextMinDomainCell(*KK);
    List L = getAvailableValues(coord, *KK);
    if (L.size == 0)
    {
        return 0;
    }
    int i;
    for (i = L.size - 1; i >= 0; i--)
    {
        KK->Result[coord.x][coord.y] = L.E[i];
        exploredCounter++;
        int boxIndex = KK->CoordBoxMapping[coord.x][coord.y];
        if (checkValidBox(boxIndex, *KK))
        {
            if (KKBackTracking(KK, Backup))
            {
                return 1;
            }
        }
        KK->Result[coord.x][coord.y] = 0;
        *Backup = *KK;
    }
    return 0;
}

int int_rand(int from, int to)
{
    int i;
    int num = (rand() % (to - from+1 )) + from;
    return num;
}

float float_rand(float min, float max)
{
    float scale = rand() / (float)RAND_MAX;
    return min + scale * (max - min);
}

void solve_KenKen_with_constraints(KenKen *KK, KenKen *Backup)
{
    *Backup = *KK;
    if (KKBackTracking(KK, Backup))
    {
        printf("\nCAN BE SOLVED WITH CONSTRAINTS ALGORITHM. EXPLORED %d STATES\n", exploredCounter);
    }
    else
    {
        printf("\nCANNOT BE SOLVED WITH CONSTRAINT ALGORITHM\n");
    }
    int i, j;

    // Xử lý Backup để sử dụng cho giải thuật giải di truyền.
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

    int count = 0;
    if (NumDigits != 9)
    {
        while (count != NumDigits / 2 + 2)
        {
            int r = int_rand(0, NumDigits - 1);
            for (i = 0; i < NumDigits; i++)
            {
                Backup->Result[r][i] = 0;
            }
            count++;
        }
    }
}

typedef struct
{
    int Chromosome[MAX_NUMDIGITS][MAX_NUMDIGITS];
    double Fitness;
} Individual;

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

typedef struct
{
    Individual Inds[MAX_INDIVIDUAL];
    int size;
} Population;

void initPopulation(Population *P)
{
    P->size = 0;
}

void appendPopulation(Population *P, Individual ind)
{
    P->Inds[P->size++] = ind;
}
void print_individual(Individual i) {
    printf("\n");
    for (int a=0; a<NumDigits; a++) {
        for (int b=0; b<NumDigits; b++) {
            printf("%d ", i.Chromosome[a][b]); 
        }
        printf("\n");
    }
}
void print_population(Population p) {
    for (int i = 0; i<p.size; i++) {
        print_individual(p.Inds[i]);
    }
}
List avalableValuesInd(int row, int col, Individual ind, KenKen KK)
{
    int values[NumDigits + 1];
    int i;
    for (i = 1; i <= NumDigits; i++)
        values[i] = 1;
    for (i = 0; i < NumDigits; i++)
    {
        if (col != i)
            values[ind.Chromosome[row][i]] = 0; // Might be redundant
        if (KK.Result[i][col] != 0 && row != i)
            values[KK.Result[i][col]] = 0;
        if (KK.Result[row][i] != 0 && col != i)
            values[KK.Result[row][i]] = 0;
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
    // Bu?c 1: T�nh BoxF.
    int i, j;
    double boxsum = 0.0;

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
    boxsum = boxsum / KK.ListBoxes.size;
    // Bu?c 2: T�nh ColF.
    int columncount[NumDigits]; // Luu t?n s? xu?t hi?n c?a m?t con s? i ? ch? s? (i-1).
    for (i = 0; i < NumDigits; i++)
        columncount[i] = 0;
    double columnsum = 0.0;
    for (i = 0; i < NumDigits; i++)
    {
        // C?p nh?t t?n s? xu?t hi?n c?a m?t con s?.
        for (j = 0; j < NumDigits; j++)
        {
            columncount[ind->Chromosome[j][i] - 1] += 1;
        }
        //�?m s? lu?ng c�c con s? c� t?n s? xu?t hi?n l?n hon 0.
        int nonzero = 0;
        for (j = 0; j < NumDigits; j++)
        {
            if (columncount[j] != 0)
                nonzero += 1;
        }
        columnsum = columnsum + ((1.0 * nonzero) / (1.0 * NumDigits));
        for (j = 0; j < NumDigits; j++)
            columncount[j] = 0;
    }
    columnsum = columnsum / NumDigits;
    // Bu?c 3: T�nh Fittness.
    if (((int)boxsum == 1) && ((int)columnsum) == 1)
    {
        ind->Fitness = 1.0;
    }
    else
    {
        ind->Fitness = (boxsum) * (columnsum);
    }
}

void seedPopulation(Population *P, int indNum, KenKen KK)
{
    
    initPopulation(P);
    int p;
    for (p = 0; p < indNum; p++)
    {
        Individual ind;
        initIndividual(&ind);
        int isValid = 1;
        int i, j;
        for (i = 0; i < NumDigits; i++)
        {
            for (j = 0; j < NumDigits; j++)
            {
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
                        int randomIndex = int_rand(0, availables.size-1 );
                        ind.Chromosome[i][j] = availables.E[randomIndex];
                    }
                }
            }
            if (isValid == 0)
            {
                p--;
                break;
            }
        }
        if (isValid == 1)
        {
            updateFitness(&ind, KK);
            appendPopulation(P, ind);
        }
    }
    sortPopulation(P);
}

Individual compete(Population P, double selectionRate)
{
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
    double ran = float_rand(0.0, 1.0);
    if (ran <= selectionRate)
        return betterInd;
    return worseInd;
}
void mutate(double mutationRate, Individual* Ind, KenKen KK){
    double r = float_rand(0, 1.0); 
    printf("%.2f ", r);
    if (r < mutationRate) {
        for (int i = 0; i<2; i++) {
            int g = int_rand(0, NumDigits-1);
            int x = int_rand(0, NumDigits-1); 
            int y = int_rand(0, NumDigits-1); 
            printf("- %d %d %d \n", g, x, y);
            int t = Ind->Chromosome[g][x]; 
            Ind->Chromosome[g][x] = Ind->Chromosome[g][y]; 
            Ind->Chromosome[g][y] = t;
        }
    }
}
Population crossover(Individual Parent1, Individual Parent2, double crossoverRate,KenKen KK) {
    printf("1");
	Population pop; 
    initPopulation(&pop); 
    Individual chidren1 = Parent1;
    Individual chidren2 = Parent2;
    double r = float_rand(0, 1.0);
	printf("%.2f", r);
    if (r < crossoverRate) {
        int soluong = int_rand(1, NumDigits);
        for (int k=0; k<soluong; k++) {
            int row = int_rand(0, NumDigits-1); 
            for (int i = 0; i<NumDigits; i++) {
                int t = chidren1.Chromosome[row][i];
                chidren1.Chromosome[row][i] = chidren2.Chromosome[row][i];
                chidren2.Chromosome[row][i] = t;
            }
        }
    }
    print_individual(chidren1); 
    appendPopulation(&pop, chidren1);
    appendPopulation(&pop, chidren2);
    return pop;
}
Individual solve_KenKen(KenKen KK)
{
	//Bước 1: Khởi tạo các biến.
	int Num_Candidates=100; //Số lượng cá thể trong quần thể.
	int Num_Elites=0.05*Num_Candidates; //Số lượng cá thể ưu tú trong quần thể.
	int Num_Generations=1000; //Số thế hệ.
	int Num_Mutations=0; //Số lần đột biến.	
	double phi=0, sigma=1, mutationRate=0.06; //Hệ số phi và hệ số sigma (để cập nhật tỉ lệ đột biến), tỉ lệ đột biến.	
	//Bước 2: Khởi tạo quần thể ban đầu.
	Population P; initPopulation(&P);
	seedPopulation(&P,Num_Candidates,KK);
	//Bước 3: Thực hiện quá trình di truyền.
	int i,stale=0;
	for (i=0;i<Num_Generations;i++)
	{
		//Kiểm tra quần thể hiện tại (kiểm tra xem đã tìm được đáp án chưa).
		printf("_Generation %d_\n",i);		
		double best_Fitness=0.0;
		sortPopulation(&P);
		if (P.Inds[0].Fitness==1) return P.Inds[0];
		if (P.Inds[0].Fitness > best_Fitness) best_Fitness=P.Inds[0].Fitness;
		printf("_Best Fitness: %.3f_\n",best_Fitness);
		//Khởi tạo một quần thể.		
		Population Next_P; initPopulation(&Next_P); 	
		Population Elites; initPopulation(&Elites); //Lấy các cá thể ưu tú để giữ lại cho thế hệ tiếp theo.
		int k;
		for (k=0;k<Num_Elites;k++) appendPopulation(&Elites,P.Inds[k]);
		int count;
		for (count=Num_Elites;count<Num_Candidates;count+=2) //Thực hiện chọn lọc tự nhiên, lai ghép và đột biến để tạo nên các cá thể mới cho thế hệ tiếp theo.
		{	
			//Lấy hai cá thể cha mẹ thông qua chọn lọc tự nhiên.
			Individual Parent1=compete(P, 1);
			Individual Parent2=compete(P, 1);			
			Population Result; initPopulation(&Result);
	
			printf("%d\n", Result.size);
			// Thực hiện lai ghép để tạo ra hai cá thể con mới.
			Result=crossover(Parent1,Parent2,1.0,KK);
			
			//Đột biến con thứ nhất.
			double old_Fitness=Result.Inds[0].Fitness;
			mutate(mutationRate,&Result.Inds[0],KK);
			updateFitness(&Result.Inds[0],KK);
		
			Num_Mutations+=1;
			if (Result.Inds[0].Fitness > old_Fitness) phi=phi+1.0;
			//Đột biến con thứ hai.			
			old_Fitness=Result.Inds[1].Fitness;
			mutate(mutationRate,&Result.Inds[1],KK);
			updateFitness(&Result.Inds[1],KK);
			
            Num_Mutations+=1;
			if (Result.Inds[1].Fitness > old_Fitness) phi=phi+1.0;
			
			appendPopulation(&Next_P,Result.Inds[0]); 
			appendPopulation(&Next_P,Result.Inds[1]);
		}
		//Tạo quần thể cho thế hệ tiếp theo.	
		for (k=0;k<Elites.size;k++) appendPopulation(&Next_P,Elites.Inds[k]);		
		P=Next_P;
		//Cập nhật hàm thích nghi cho mọi cá thể và sắp xếp quần thể mới vừa tạo.
		for (k=0;k<P.size;k++) updateFitness(&P.Inds[k],KK);
		sortPopulation(&P);	
		//Cập nhật tỉ lệ đột biến.
		if (Num_Mutations==0) phi=0;
		else phi=phi/(1.0*Num_Mutations);
		if (phi>0.2) sigma=sigma/0.998;
		else sigma=sigma*0.998;
		mutationRate=float_rand(0.0,sigma);		
		//Kiểm tra quần thể mới vừa tạo (kiểm tra xem có bị bế tắc từ 100 lần trở lên hay không).
		if (P.Inds[0].Fitness != P.Inds[1].Fitness) stale=0;
		else stale+=1;
		if (stale>=10) //Nếu bị bế tắc, khởi tạo lại quần thể từ ban đầu.
		{
			printf("Stale Population. Re-seeding...\n");
			seedPopulation(&P,Num_Candidates,KK);
			stale=0;
			sigma=1;
			phi=0;
			Num_Mutations=0;
			mutationRate=0.06;
		}			
	}
}

int main()
{
	srand(time(0));
	KenKen KK; readKenKen(&KK,"input5x5.txt");
	KenKen Backup;
	// printKenKen(KK);
	//solve_KenKen_with_constraints(&KK,&Backup);
	Population p; 
	initPopulation(&p); 
	seedPopulation(&p, 2, KK);
	p = crossover(p.Inds[0], p.Inds[1], 1, KK);
	printf(" -- ");
	print_individual(p.Inds[0]);	
	Individual KQ=solve_KenKen(KK);
	print_individual(KQ);	
	return 0;
}
