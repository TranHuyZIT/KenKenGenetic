import copy
import random
import time

import matplotlib.pyplot as plt
import numpy as np
import pygame

MAX_INDIVIDUALS = 1050
MAX_LENGTH = 81
MAX_NUMDIGITS = 9
NumDigits = 9


# Init Pygame
# Board Width
WIDTHGAME = 720
HEIGHT = WIDTHGAME
CELL_WIDTHGAME = 80

# Menu Width
MENU = 220
KENKENX = 0
KENKENY = 0
CLOCK_X = MENU // 2
CLOCK_Y = 500


# Pygame sound init
pygame.mixer.pre_init(44100, -16, 2, 2)
pygame.mixer.init()
pygame.init()
pygame.mixer.quit()
pygame.mixer.init(22050, -16, 2, 2)

clickSound = pygame.mixer.Sound("clickSound.wav")
backgroundMusic = pygame.mixer.Sound("backgroundMusic.mp3")
pygame.mixer.Sound.set_volume(backgroundMusic, 0.6)
successSound = pygame.mixer.Sound("success.mp3")
solveSound = pygame.mixer.Sound("solveSound.mp3")


# Color RGB
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 255, 0)
WHITE = (255, 255, 255)
MAIN = (157,109,66)
SECONDARY = (227,181,142)
BACKGROUND = (239,221,206)
BTN_HOV = SECONDARY
BTN = MAIN
BLACK = (0, 0, 0)
GRIDLINE = WHITE
GRIDLINETHIN = (70, 100, 140)
ORANGE = (255, 165 ,0)
GREY = (128, 128, 128)
TURQUOISE = (64, 224, 208)


def findNeighbourUp(source, coords):
    if source.x == 0:
        return False
    for coord in coords:
        if coord.x == source.x - 1 and coord.y == source.y:
            return True
    return False


def findNeighbourDown(source, coords):
    if source.x == NumDigits - 1:
        return False
    for coord in coords:
        if coord.x == source.x + 1 and coord.y == source.y:
            return True
    return False


def findNeighbourLeft(source, coords):
    if source.y == 0:
        return False
    for coord in coords:
        if coord.y == source.y - 1 and coord.x == source.x:
            return True
    return False


def findNeighbourRight(source, coords):
    if source.y == NumDigits - 1:
        return False
    for coord in coords:
        if coord.y == source.y + 1 and coord.x == source.x:
            return True
    return False

COLORMAPPING = {
    0: (61,34,17),
    1: (73,42,21),
    2: (95,55,20),
    3: (121,75,38),
    4: (79,32,22),
    5: (69,29,19),
    6: (64,25,18),
    7: (53,23,15),
    8: (46,19,12),
    9: (39,15,11),
}


class Coord:
    def __init__(self, x, y=-1):
        if y == -1:
            self.x = x // NumDigits
            self.y = x % NumDigits
        else:
            self.x = x
            self.y = y

    def indexOf(self):
        return self.x * NumDigits + self.y

    def draw(self, box, value, color, WIN, isTopCoord=False):
        x = self.y * CELL_WIDTHGAME + KENKENX
        y = self.x * CELL_WIDTHGAME + KENKENY
        fontBig = pygame.font.SysFont('comicsans', 30)
        if int(value) == 0: value = ''
        textBig = fontBig.render(str(value), True, WHITE)

        pygame.draw.rect(WIN, color, (x, y, CELL_WIDTHGAME, CELL_WIDTHGAME))
        if not findNeighbourUp(self, box.coords):
            pygame.draw.line(WIN, GRIDLINE, (x, y), (x + CELL_WIDTHGAME, y), 5)
        if not findNeighbourDown(self, box.coords):
            pygame.draw.line(WIN, GRIDLINE, (x, y + CELL_WIDTHGAME), (x + CELL_WIDTHGAME, y + CELL_WIDTHGAME), 5)
        if not findNeighbourLeft(self, box.coords):
            pygame.draw.line(WIN, GRIDLINE, (x, y), (x, y + CELL_WIDTHGAME), 5)
        if not findNeighbourRight(self, box.coords):
            pygame.draw.line(WIN, GRIDLINE, (x + CELL_WIDTHGAME, y), (x + CELL_WIDTHGAME, y + CELL_WIDTHGAME), 5)
        if isTopCoord:
            fontSmall = pygame.font.SysFont('comicsans', 16)
            textSmall = fontSmall.render(f'{box.value}, {box.op}', True, WHITE)
            WIN.blit(textSmall, (x + 10, y + 5))
        WIN.blit(textBig,
                 (x + (CELL_WIDTHGAME / 2 - textBig.get_width() / 2), y + (CELL_WIDTHGAME / 2 - textBig.get_height() / 2)))


class Constrains:
    def __init__(self, n):
        self.data = []
        for i in range(n * n):
            arrRow = []
            for j in range(n * n):
                arrRow.append(0)
            self.data.append(arrRow)
        self.n = n * n

    def addConstrain(self, source, target):
        u = source.indexOf()
        v = target.indexOf()
        self.data[u][v] = 1

    def spreadConstrains(self, coord):
        for j in range(NumDigits):
            if j != coord.y:
                target = Coord(coord.x, j)
                self.addConstrain(coord, target)
        for i in range(NumDigits):
            if i != coord.x:
                target = Coord(i, coord.y)
                self.addConstrain(coord, target)


class Box:
    def __init__(self):
        self.coords = []
        self.op = ''
        self.value = 0


class KenKen:
    def __init__(self):
        self.CoordBoxMapping = []
        self.Result = []
        self.ListBoxes = []
        self.C = Constrains(NumDigits)
        self.exploredStates = 0
        self.MAX_EXPLORED = 500

    def draw(self, WIN):
        for i in range(len(self.ListBoxes)):
            box = self.ListBoxes[i]
            color = COLORMAPPING[i % 10]
            sorted(box.coords, key=lambda coord: coord.y)
            coord = box.coords[0]
            coord.draw(box, self.Result[coord.x][coord.y], color, WIN, isTopCoord=True)
            for j in range(1, len(box.coords)):
                coord = box.coords[j]
                coord.draw(box, self.Result[coord.x][coord.y], color, WIN)

    def emptyResult(self):
        for i in range(NumDigits):
            for j in range(NumDigits):
                self.Result[i][j] = 0

    def initKenKen(self, n):
        self.exploredStates = 0
        self.C = Constrains(n)
        for i in range(NumDigits):
            for j in range(NumDigits):
                coord = Coord(i, j)
                self.C.spreadConstrains(coord)

    def readFile(self, name):
        with open(name, "r") as f:
            print("Open file " + name)
            global NumDigits
            global WIDTHGAME
            NumDigits = int(f.readline().strip())
            global KENKENX, KENKENY
            KENKENX = ((WIDTHGAME) - (NumDigits * CELL_WIDTHGAME)) // 2 + MENU
            KENKENY = (HEIGHT - NumDigits*CELL_WIDTHGAME) // 2
            for i in range(NumDigits):
                arrRowCBM = []
                arrRowR = []
                for j in range(NumDigits):
                    arrRowCBM.append(-1)
                    arrRowR.append(0)
                self.CoordBoxMapping.append(arrRowCBM)
                self.Result.append(arrRowR)
            boxesNum = int(f.readline().strip())
            for i in range (boxesNum):
                line = f.readline().strip()
                box = Box()
                coordsNum = int(line[0])
                for j in range(coordsNum):
                    coordsData = line[j*4 + 1: (j*4 + 1) + 4].strip()
                    coord = Coord(int(coordsData[0]), int(coordsData[2]))
                    self.CoordBoxMapping[coord.x][coord.y] = i
                    box.coords.append(coord)
                boxInfo = line[(j*4 + 1) + 4:].strip()
                box.op = boxInfo[0]
                box.value = int(boxInfo[2:])
                self.ListBoxes.append(box)
            f.close()
        self.initKenKen(NumDigits)

    def isFilledKenKen(self):
        for i in range(NumDigits):
            for j in range(NumDigits):
                if self.Result[i][j] == 0:
                    return 0
        return 1

    def getAvailableValues(self, coord):
        values = [0] * (NumDigits + 1)
        inBox = self.CoordBoxMapping[coord.x][coord.y]
        op = self.ListBoxes[inBox].op
        valueBox = self.ListBoxes[inBox].value
        sizeBox = len(self.ListBoxes[inBox].coords)

        if op == "=":
            values[valueBox] = 1
        elif op == '/':
            values[1] = 1
            values[valueBox] = 1
            for i in range(NumDigits, NumDigits // 2, -1):
                for j in range(NumDigits // 2, 0, -1):
                    if (i /j == valueBox) and (i % j == 0):
                        values[i] = 1
                        values[j] = 1
        elif (op == '-') and (sizeBox == 2):
            for i in range(1, NumDigits + 1):
                if i + valueBox > NumDigits: break
                values[i] = 1
                values[i + valueBox] = 1
        elif op == '+':
            if sizeBox == 2:
                for i in range(1, NumDigits + 1):
                    for j in range(1, NumDigits + 1):
                        if (i != j) and (i + j == valueBox):
                            values[i] = 1
                            values[j] = 1
            else:
                for i in range(1, NumDigits + 1):
                    values[i] = 1
        elif op == 'x':
            if sizeBox == 2:
                for i in range(1, NumDigits + 1):
                    for j in range(1, NumDigits + 1):
                        if (i != j) and (i * j == valueBox):
                            values[i] = 1
                            values[j] = 1
            else:
                for i in range(1, NumDigits + 1):
                    values[i] = 1
        vertex = coord.indexOf()
        for i in range(self.C.n):
            if self.C.data[vertex][i] == 1:
                temp = Coord(i)
                values[self.Result[temp.x][temp.y]] = 0

        L = []
        for i in range(1, NumDigits + 1):
            if values[i] == 1:
                L.append(i)
        return L

    def getNextMinDomainCell(self):
        minVal = float('inf')
        minCell = Coord(0, 0)
        for i in range(NumDigits):
            for j in range(NumDigits):
                if self.Result[i][j] == 0:
                    coord = Coord(i, j)
                    if len(self.getAvailableValues(coord)) < minVal:
                        minCell = coord
                        minVal = len(self.getAvailableValues(coord))
        return minCell

    def isFilledBox(self, boxIndex):
        box = self.ListBoxes[boxIndex]
        for i in range(len(box.coords)):
            coord = box.coords[i]
            if self.Result[coord.x][coord.y] == 0:
                return 0
        return 1

    def checkValidBox(self, boxIndex):
        if self.isFilledBox(boxIndex):
            box = self.ListBoxes[boxIndex]
            op = box.op
            boxValue = box.value
            s = 0
            if op == 'x' or op == '/':
                s = 1
            for coord in box.coords:
                if op == 'x':
                    s *= self.Result[coord.x][coord.y]
                elif op == '/':
                    s = max(s, self.Result[coord.x][coord.y]) / min(s, self.Result[coord.x][coord.y])
                elif op == '=':
                    s = self.Result[coord.x][coord.y]
                elif op == '+':
                    s += self.Result[coord.x][coord.y]
                elif op == '-':
                    s = abs(s - self.Result[coord.x][coord.y])
            return s == boxValue
        return 1

    def print(self):
        for i in range(NumDigits):
            print(self.Result[i])

    def KKBacktracking(self, menu, WIN):
        if self.isFilledKenKen():
            pygame.mixer.Sound(successSound)
            print("Solved with Constraint Satisfaction")
            self.print()
            return 1

        coord = self.getNextMinDomainCell()
        L = self.getAvailableValues(coord)
        if len(L) == 0:
            return 0
        for i in range(len(L) - 1, -1, -1):
            self.exploredStates += 1
            if self.exploredStates > self.MAX_EXPLORED:
                raise Exception("Explored states exceeded limit for CP Algorithm")
            print(self.exploredStates)
            self.Result[coord.x][coord.y] = L[i]
            draw(self, menu, WIN)
            boxIndex = self.CoordBoxMapping[coord.x][coord.y]
            if self.checkValidBox(boxIndex):
                if self.KKBacktracking(menu, WIN):
                    break
            self.Result[coord.x][coord.y] = 0
            global backup
            backup = copy.deepcopy(self)

        return 0

    def solveWithConstraint(self, menu, WIN):
        try:
            if self.KKBacktracking(menu, WIN):
                print("Solved with constraint satisfaction")
                return True
            else:
                for i in range(len(self.ListBoxes)):
                    if not backup.isFilledBox(i):
                        for coord in self.ListBoxes[i].coords:
                            backup.Result[coord.x][coord.y] = 0
                return False
        except:
            for i in range(len(self.ListBoxes)):
                if not backup.isFilledBox(i):
                    for coord in self.ListBoxes[i].coords:
                        backup.Result[coord.x][coord.y] = 0
            return False



    def isValidColumn(self, val, colIndex):
        for row in self.Result:
            if row[colIndex] == val:
                return False
        return True

class Individual:
    def __init__(self):
        self.fitness = 0
        self.Chromosome = []
        for i in range(NumDigits):
            arrRow = []
            for j in range(NumDigits):
                arrRow.append(0)
            self.Chromosome.append(arrRow)

    def __gt__(self, other):
        return self.fitness > other.fitness

    def __lt__(self, other):
        return self.fitness <= other.fitness

    def printIndividual(self):
        print(f'Individual with fitness {self.fitness}:')
        for row in self.Chromosome:
            print(row)

    def availableValuesInd(self, row, col, KK):
        values = []
        for i in range(0, NumDigits + 2):
            values.append(1)
        for i in range(NumDigits):
            if col != i:
                values[self.Chromosome[row][i]] = 0
            if KK.Result[i][col] != 0 and row != i:
                values[KK.Result[i][col]] = 0
            if KK.Result[row][i] != 0 and col != i:
                values[KK.Result[row][i]] = 0
        L = []
        for i in range(1, NumDigits + 1):
            if values[i]:
                L.append(i)
        return L

    def updateFitness(self, KK):
        boxF = 0
        colF = 0
        for box in KK.ListBoxes:
            op = box.op
            conValue = box.value
            S = 0
            if op == 'x' or op == '/':
                S = 1
            for coord in box.coords:
                if op == 'x':
                    S *= self.Chromosome[coord.x][coord.y]
                elif op == '/':
                    S = max(S, self.Chromosome[coord.x][coord.y]) / min(S, self.Chromosome[coord.x][coord.y])
                elif op == '=':
                    S = self.Chromosome[coord.x][coord.y]
                elif op == '+':
                    S += self.Chromosome[coord.x][coord.y]
                elif op == '-':
                    S = abs(S - self.Chromosome[coord.x][coord.y])
            if S == conValue:
                boxF += 1
        boxF /= len(KK.ListBoxes)
        columnCount = []
        for i in range(1, NumDigits + 1):
            columnCount.append(0)
        for i in range(NumDigits):
            for j in range(NumDigits):
                columnCount[self.Chromosome[j][i] - 1] += 1
            nonZero = 0
            for j in range(NumDigits):
                if columnCount[j] != 0:
                    nonZero += 1
            colF += nonZero / NumDigits
            for j in range(NumDigits):
                columnCount[j] = 0
        colF /= NumDigits
        if (int(boxF) == 1) and (int(colF) == 1):
            self.fitness = 1
        else:
            self.fitness = boxF * colF

    def mutate(self, mutationRate, KK):
        randomFloat = random.random()
        numberMutateCell = 2
        if randomFloat < mutationRate:
            for i in range(numberMutateCell):
                row = random.randrange(NumDigits)
                col1 = random.randrange(NumDigits)
                col2 = random.randrange(NumDigits)
                if (KK.isValidColumn(self.Chromosome[row][col2], col1)) and (KK.isValidColumn(self.Chromosome[row][col1], col2)):
                    self.Chromosome[row][col1], self.Chromosome[row][col2] = self.Chromosome[row][col2], self.Chromosome[row][col1]


class Genetic:
    def __init__(self, menu, WIN):
        self.bestIndividual = Individual()
        self.menu = menu
        self.WIN = WIN

    def compete(self, P, selectionRate):
        ind1 = P.Inds[random.randrange(len(P.Inds))]
        ind2 = P.Inds[random.randrange(len(P.Inds))]
        if ind1 > ind2:
            betterInd, worseInd = ind1, ind2
        else:
            betterInd, worseInd = ind2, ind1
        randomFloat = random.random()
        if randomFloat < selectionRate:
            return betterInd
        return worseInd

    def crossOver(self, parent1, parent2, crossOverRate, KK):
        pop = Population()
        children1 = copy.deepcopy(parent1)
        children2 = copy.deepcopy(parent2)
        randomFloat = random.random()
        if randomFloat < crossOverRate:
            numberRow = random.randrange(NumDigits)
            for k in range(numberRow):
                row = random.randrange(NumDigits)
                for i in range(NumDigits):
                    tmp = children1.Chromosome[row][i]
                    children1.Chromosome[row][i] = children2.Chromosome[row][i]
                    children2.Chromosome[row][i] = tmp
        pop.appendPopulation(children1)
        pop.appendPopulation(children2)
        return pop

    def solve(self, KK):
        NUM_CANDIDATES = 100
        NUM_ELITES = 0.05 * NUM_CANDIDATES
        NUM_GENERATION = 1000
        MUTATION_RATE = 0.06
        SELECTION_RATE = 0.85
        CROSSOVER_RATE = 0.9
        STALE_MAX = 100
        stale = 0
        P = Population()
        P.seedPopulation(NUM_CANDIDATES, KK)
        print("Initial Population")
        P.printPopulation()
        for i in range(NUM_GENERATION):
            print(f'Generation{i}')
            bestFitness = 0
            P.sortPopulation()
            if P.Inds[0].fitness == 1:
                return P.Inds[0]
            if P.Inds[0].fitness > bestFitness:
                bestFitness = P.Inds[0].fitness
            if self.bestIndividual.fitness < bestFitness:
                self.bestIndividual = P.Inds[0]
            bestFitnesses.append(self.bestIndividual.fitness)
            ken = copy.deepcopy(KK)
            ken.Result = P.Inds[0].Chromosome
            draw(ken, self.menu, self.WIN)
            nextPopulation = Population()
            elitesPopulation = Population()
            for k in range(int(NUM_ELITES)):
                elitesPopulation.appendPopulation(P.Inds[k])
                print(k)
            for count in range(int(NUM_ELITES), NUM_CANDIDATES, 2):
                parent1 = self.compete(P, SELECTION_RATE)
                parent2 = self.compete(P, SELECTION_RATE)
                result = self.crossOver(parent1, parent2, CROSSOVER_RATE, KK)
                oldFitness = result.Inds[0].fitness
                result.Inds[0].mutate(MUTATION_RATE, KK)
                result.Inds[0].updateFitness(KK)
                if result.Inds[0].fitness > oldFitness:
                    oldFitness = result.Inds[1].fitness
                result.Inds[1].mutate(MUTATION_RATE, KK)
                result.Inds[1].updateFitness(KK)

                if result.Inds[1].fitness > oldFitness:
                    nextPopulation.appendPopulation(result.Inds[0])
                nextPopulation.appendPopulation(result.Inds[1])

            for ind in elitesPopulation.Inds:
                nextPopulation.appendPopulation(ind)
            print(f'Best Fitness: {bestFitness}')
            ken = copy.deepcopy(KK)
            ken.Result = P.Inds[0].Chromosome
            draw(ken, self.menu, self.WIN)
            P = nextPopulation
            for ind in P.Inds:
                ind.updateFitness(KK)
            P.sortPopulation()
            if P.Inds[0].fitness != P.Inds[1].fitness:
                stale = 0
            else:
                stale += 1
            if stale > STALE_MAX:
                P.seedPopulation(NUM_CANDIDATES, KK)
        return P.Inds[0]


class Population:
    def __init__(self):
        self.Inds = []

    def appendPopulation(self, ind):
        self.Inds.append(ind)

    def printPopulation(self):
        print("- Population:")
        for ind in self.Inds:
            ind.printIndividual()

    def sortPopulation(self):
        self.Inds.sort(key=lambda x: x.fitness, reverse=True)

    def seedPopulation(self, indNum, KK):
        self.Inds.clear()
        for p in range(indNum):
            ind = Individual()
            isValid = 1
            for i in range(NumDigits):
                for j in range(NumDigits):
                    if KK.Result[i][j] != 0:
                        ind.Chromosome[i][j] = KK.Result[i][j]
                    else:
                        availables = ind.availableValuesInd(i, j, KK)
                        if len(availables) == 0:
                            isValid = 0
                            break
                        else:
                            randomIndex = random.randrange(len(availables))
                            ind.Chromosome[i][j] = availables[randomIndex]
                if not isValid:
                    p -= 1
                    break
            if isValid:
                ind.updateFitness(KK)
                self.appendPopulation(ind)
        self.sortPopulation()


class Text:
    def __init__(self, x, y, text, textColor, fontSize, font='comicsans'):
        self.x = x
        self.y = y
        self.text = text
        self.textColor = textColor
        self.fontSize = fontSize
        self.font = font

    def draw(self, win):
        font = pygame.font.SysFont(str(self.font), self.fontSize)
        text = font.render(self.text, True, self.textColor)
        win.blit(text,
                 (self.x, self.y))


class Button:
    def __init__(self, color, textColor, x, y,width, height,text = ''):
        self.color = color
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.text = text
        self.textColor = textColor

    def draw(self,win, outline=None):
        if outline:  # Draw ouline
            pygame.draw.rect(win, outline, (self.x - 2,self.y - 2 ,self.width + 4, self.height + 4),0)
        pygame.draw.rect(win, self.color,(self.x, self.y, self.width,self.height), 0)
        if self.text != "":
            font = pygame.font.SysFont('comicsans',30)
            text = font.render(self.text,True,self.textColor)
            win.blit(text, (self.x + (self.width/2 - text.get_width()/ 2), self.y + (self.height / 2 - text.get_height() / 2)))

    def is_over(self,pos):
        if pos[0] > self.x and pos[0] < self.x + self.width:
            if pos[1] > self.y and pos[1] < self.y + self.height:
                self.textColor = BLACK
                return True
        self.textColor = WHITE
        return False


class Menu:
    def __init__(self):
        self.x = MENU
        self.y = WIDTHGAME
        self.bgColor = GREY
        self.btnColor = BTN
        self.start = Button(self.btnColor, WHITE, 30, 140, 140, 80, 'Start')
        self.empty = Button(self.btnColor, WHITE, self.start.x, self.start.y + 100, 140, 80, 'Clear')
        self.exit = Button(self.btnColor, WHITE, self.empty.x, self.empty.y + 100, 140, 80, 'Exit')

    def draw(self, WIN):
        font = pygame.font.SysFont('comicsans', 60)
        text = font.render('MENU', True, MAIN)
        WIN.blit(text, (20, 20, 100, 100))
        self.start.draw(WIN, BLACK)
        self.empty.draw(WIN, BLACK)
        self.exit.draw(WIN, BLACK)

    def checkHover(self, pos):
        if self.start.is_over(pos):
            self.start.color = BTN_HOV
        else:
            self.start.color = self.btnColor

        if self.empty.is_over(pos):
            self.empty.color = BTN_HOV
        else:
            self.empty.color = self.btnColor

        if self.exit.is_over(pos):
            self.exit.color = BTN_HOV
        else:
            self.exit.color = self.btnColor


def draw(KK, menu, WIN):
    global timer
    timer = pygame.time.get_ticks() // 1000
    secText = Text(CLOCK_X, CLOCK_Y, str(timer), WHITE, 60)
    WIN.fill(BACKGROUND)
    menu.draw(WIN)
    KK.draw(WIN)
    secText.draw(WIN)
    pygame.display.update()


timer = 0

backup = KenKen()
generations = [i for i in range(1000)]
bestFitnesses = []
def main():
    run = True
    menu = Menu()
    ken = KenKen()
    ken.readFile('input7.txt')
    WIN = pygame.display.set_mode((WIDTHGAME + MENU, HEIGHT))
    pygame.display.set_caption("Solve KenKen With Genetic")
    pygame.mixer.Sound.play(backgroundMusic, loops=-1)

    while run:
        draw(ken, menu, WIN)
        for event in pygame.event.get():
            m_pos = pygame.mouse.get_pos()
            menu.checkHover(m_pos)
            if event.type == pygame.QUIT:
                run = False
            if pygame.mouse.get_pressed()[0]:
                pygame.mixer.Sound.play(clickSound)
                if menu.start.is_over(m_pos):
                    global timer
                    timer = 0
                    pygame.mixer.Sound.play(solveSound, loops=-1)
                    if ken.solveWithConstraint(menu, WIN):
                        pygame.mixer.Sound.play(successSound)
                        pygame.mixer.Sound.stop(solveSound)
                    else:
                        ken.print()
                        print()
                        backup.print()
                        backup.draw(WIN)
                        pygame.display.update()
                        pygame.mixer.Sound.play(successSound)
                        pygame.mixer.Sound.stop(solveSound)
                        time.sleep(2)
                        pygame.mixer.Sound.play(solveSound, loops=-1)
                        geneticAlgo = Genetic(menu, WIN)
                        geneticAlgo.solve(backup)
                        pygame.mixer.Sound.play(successSound)
                        pygame.mixer.Sound.stop(solveSound)
                        geneticAlgo.bestIndividual.printIndividual()
                        ken.Result = geneticAlgo.bestIndividual.Chromosome
                        draw(ken, menu, WIN)
                        plt.xlabel('Generations')
                        plt.ylabel('Fitness')
                        plt.plot(generations, bestFitnesses)
                        plt.show()

                elif menu.empty.is_over(m_pos):
                    ken.emptyResult()
                elif menu.exit.is_over(m_pos):
                    run = False


if __name__ =="__main__":
    main()








