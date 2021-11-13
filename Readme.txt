##############################################
#
# I53 - Compilation et théorie des langages
# ----------
#
# TP4 - Automates (partie 2)
#
# BENJELLOUN Youssef / LEAL André / Groupe 1.A
# 14-11-21
###############################################

L'archive contient les fichiers suivants que nous allons détailler :

- Scanner.c
- Parser.c
- Regcomp.c
- Regcomp.h
- afd.c
- afd.h
- afn.c
- afn.h
- stack.c
- stack.h
- mygrep.c
- makefile


Nous avons utilisé la grammaire suivante :

Expr -> Terme Reste_e

Reste_e -> + Terme Reste_e
        | epsilon

Terme -> Fact Reste_t

Reste_t -> . Fact Reste_t
        | epsilon

Fact  -> Reste_F*
      |  Reste_F

Reste_F -> CHAR
        | ( Expr )


################################## SCANNER.C ##################################

Il s'agit de l'analyseur lexical. Il va analyser la chaîne de caractères et
renvoyer une liste d'unité lexicales se distinguant dans 7 catégories parmis le
lexique suivant :

PO  --> '('
PF  --> ')'
OP  --> {'.','*','+'}
CO  --> '['
CF  --> ']'
AO  --> '{'
AF  --> '}'

- Les opérateurs "OP" : spécifiés dans la liste OP en suivant l'ordre de
                        priorité.

- Les parenthèses ouvrantes "PO" : identifiées en tant que caractère (str).

- Les parenthèses fermantes "PF" : identifiées en tant que caractère (str).

- Facultatif : Ajout des acolades ouvrantes "AO" et fermantes "AF" identifiées
               en tant que caractère (str) pour modéliser Exreg{n} qui permet la
               répétition n fois d'une expression régulière. (NB : un seul
               chiffre est accepté par paire d'acolades).

- Facultatif : Ajout des crochets ouvrants "CO" et fermants "CF"
               identifiés en tant que caractère (str) pour modéliser
               [c1c2...cn] qui represente exactement un des
               caractères ci.


Si un caractère est reconnu, il est ajouté à la liste d'unité lexicales
"arr_ul" qui sera renvoyée à la fin de la fonction.

Sinon, une erreur est explicitée avec le caractère fautif en question.

--> Les sauts de lignes et les espaces sont ignorés.


################################## PARSER.C  ##################################

Il s'agit de l'analyseur syntaxique. Il va prendre en entrée la liste d'unités
lexicales "arr_ul" générée par la fonction scanner et réaliser les actions
suivantes :

- Si la chaîne en entrée est une expression de taille conforme :
---> Elle est déroulée selon la grammaire pré-établie jusqu'à avoir sa structure
 finale.
---> Elle est retournée en notation polonaise inversée (postfixe)

- Sinon il affiche deux erreurs possibles :
1/ La chaîne n'a pas été lue en entier
2/ Erreur syntaxique


EXEMPLE :


################################## REGCOMP.C ##################################

Reprend le fonctionnement du fichier Codegen du TP2. Il prend en entrée une
expression postfixee et produit un fichier C qui contient un code à trois
adresses pour traduire l'évaluation de l'expression rangés avec une pile.

(Affiche également le message "compilé avec succès" si la simulation de la pile
est un succès.)


#################################### AFD.C ####################################
#################################### AFN.C ####################################
################################### STACK.C ###################################


################################### MYGREP.C ##################################

--> La ligne de commande pour lancer le fichier est :
    ./mygrep <ExprReg> <Chaîne>

Assemble dans un main le déroulement successif des fonctions scanner, parser et
regcomp (codegen) ainsi que la simulation d'un AFD pour afficher si oui ou non
la chaîne entrée appartient au langage dénoté par l'expression régulière.

EXEMPLE : de compilation d'un fichier nommé exemple1 :




(- Exemples d'utilisation :
 - Remarques :)
