# I53 Automates (BENJELLOUN Youssef, LEAL Andre)

## Utilisation
```bash
./mygrep <ExprReg> <Chaîne>
```
## Description 
L'archive contient les fichiers suivants que nous allons détailler :

- scanner.c/.h : L'analyseur lexical
- parser.c/.h  : L'analyseur syntaxique qui transforme la chaine en "postfixée"
- regcomp.c/.h : Chargé de générer un automate fini deterministe à partir d'une expression "postfixée"
- afd.c/.h     : Implémentation des automate fini déterministe.
- afn.c/.h     : Implémentation des automate fini non-déterministe.
- stack.c/.h   : Implémentation d'une pile standard avec des AFN
- mygrep.c     : Chargé de faire appel aux modules pour essayer de reconnaître la chaine
- makefile     : Fichier makefile

### **mygrep.c**

La ligne de commande pour l'exécution est la suivante :
```bash
./mygrep <ExprReg> <Chaîne>
```
Assemble dans un main le déroulement successif des fonctions scanner, parser et
regcomp pour simuler un AFD et afficher si oui ou non la chaîne entrée 
appartient au langage dénoté par l'expression régulière.

### **scanner.c**

Il s'agit de l'analyseur lexical. Il va analyser la chaîne de caractères et
renvoyer une liste d'unité lexicales se distinguant dans 8 catégories parmis le
lexique suivant :
```
 CHAR --> {a-z} {A-Z} {0-9}
 PO  --> '('
 PF  --> ')'
 OP  --> {'.','*','+'}
 CO  --> '['
 CF  --> ']'
 AO  --> '{'
 AF  --> '}'
 ```

- Les caractères "CHAR" : reconnus selon la convention adoptée en cours via la fonction "is_char".
- Les opérateurs "OP" : identifiés en tant que caractère et spécifiés dans la liste OP grace.
- Les parenthèses ouvrantes "PO" : identifiées en tant que caractère.
- Les parenthèses fermantes "PF" : identifiées en tant que caractère.
- Facultatif : Ajout des accolades ouvrantes "AO" et fermantes "AF" identifiées en tant que caractère (str) pour modéliser Exreg{n} qui permet la répétition n fois d'une expression régulière.
 (Remarque : pour **{x}** on accepte x tel que 1<=x<=9 soit un chiffre positif par paire d'accolades).
- Facultatif : Ajout des crochets ouvrants "CO" et fermants "CF" identifiés en tant que caractère pour modéliser [c1,c2,...cn] qui represente exactement un des caractères ci.


Si un caractère est reconnu, il est ajouté à la liste d'unité lexicales
"arr_ul" qui sera renvoyée à la fin de la fonction. Sinon, une erreur est explicitée avec le caractère fautif en question.

Les sauts de lignes et les espaces ne sont pas ignorés car peuvent porter à confusion.


### **parser.c**
Nous avons utilisé la grammaire suivante :

```c
Expr -> Terme Reste_e

Reste_e -> + Terme Reste_e
         | epsilon
       
Terme -> Fact Reste_t

Reste_t -> . Fact Reste_t
         | epsilon

Fact  -> Reste_F*
       |  Reste_F REP
       |  Reste_F

Rep   -> {n} REP
       | epsilon

Reste_F -> CHAR
         | ( Expr )
         | [char *]    //chaîne de caractère
```
       

Il s'agit de l'analyseur syntaxique. Il va prendre en entrée la liste d'unités
lexicales "arr_ul" générée par la fonction scanner et réaliser les actions
suivantes :
* Si la liste d'unités lexicales en entrée est une expression conforme :
    * Elle est déroulée selon la grammaire pré-établie jusqu'à avoir sa structure finale.
    * Elle est retournée en "notation polonaise inversée" (postfixe)

* Sinon il affiche **ERREUR SYNTAXIQUE** en précisant le caractère fautif. 

Remarque : Ajout de {n} REP pour pouvoir reconnaître exp{x}{y}{z}.

Remarque : Les règles de passage en notation polonaise inversée s'appliquent comme pour les expressions arithmétiques et booléennes sauf pour : 
* Les crochets `[]` qui ne changent pas de position. En effet, il est nécessaire de garder l'information du début et de la fin de la chaîne.
* Les accolades `{}` qui sont supprimées car la visualisation des chiffres est suffisante. 
```
EXEMPLE :  (a+b).[ac]{3}  ------>  ab+[ac].3
              INFIXE                POSTFIXE
```
Remarque : Si vous avez besoin d'enchainer des `{}` et `*` (par example `(a.b){2}*`), il faudra ajouter de parenthèses devant la première expression `((a.b){2})*`

### **regcomp.c**
Prend une expression postfixée en paramètre et construit un AFN reconnaissant 
cette expression. 
### **afd.c**
Implémentation de toutes les fonctions nécessaires à la mise en place d'un automate fini déterministe. 


### **afn.c**
Implémentation de toutes les fonctions nécessaires à la mise en place d'un automate fini non-déterministe. 


### **stack.c**
Implémentation d'une pile avec des AFN, en ayant à disposition les fonctions empiler / dépiler et affichage du sommet de la pile.

## Exemples
```bash
$ ./mygrep '((a+b).(c+d))*.(c+d)' 'acbdc'
acceptée 
```

```bash
$ ./mygrep '(a+b.c+d)*.(c+d)' ''
rejetée
```

```bash
$ ./mygrep '(a+b.c+d)*.(c+d)' 'ab'
rejetée
```

```bash
$ ./mygrep '(a*.b).[ab]{4}' 'aaababababab'
acceptée  
```

```bash
$ ./mygrep '(a*.b).[ab]{4}' 'bbabababab' 
rejetée
```
```bash
./mygrep '(s+t+o+u+e)*.[ok]{2}{2}' 'toutestokokokokok'
acceptée
```
```bash
./mygrep '(s+t+o+u+e)*.[ok]{2}{2}' 'ouokokokokou'
rejetée
```
```bash
./mygrep '((a.b){2})*' 'abababab'
acceptée
```
# BENJELLOUN Youssef / LEAL André / Groupe 1.A
