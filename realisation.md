BOM
===

* ATTiny2313A -- 0.66€/100 -- Farnell ref 1841616 [ATTINY2313A-PU]  66€
* Pull-up http://fr.farnell.com/multicomp/mcre000073/resistance-carbon-film-125mw-1m/dp/1700277
* CR2032
  * http://www.amazon.fr/Panasonic-Piles-Boutons-Lithium-CR2032/dp/B002EVOA5E
* LED:
  * http://fr.farnell.com/kingbright/l-9294surck/led-wide-view-130-rouge/dp/2079968 11.10€
  * http://fr.farnell.com/kingbright/l-9294syck/led-wide-view-130-jaune/dp/2079973 11.10€
* carton ou bristol comme support
* le support de pile sera en carton! avec du coton hydrophyle pour padder
* fil nu où nécessaire (pas besoin de denuder le fil!)
* adhesive copper tape (12mm / 16m = 21.37€) Farnell ref 1653450
* papier alu (pour le touchpad -- la mine de crayon ne fonctionne pas)
* épingles pour fixer sur un vêtement
* sachets pour donner aux gens

Outillage
=========

* clous pour percer le printer paper board (je cite)
* marteaux
* paires de ciseau
* fers a souder, éponges, soudure, rallonges électriques
* glue gun, glue sticks

Commande Farnell
================

Idée: on fait 80 kits à 2€ avec une LED jaune et une rouge, et 20 kits à 3€ avec 8 rouges?

    ref     qty total (HT)
    1841616 100  66.00     ATTiny2313A DIP
    1700277 100   1.30     Resistance 1M-ohm
    2065171 100  48.00     CR2032
    2079973 100  11.10     LED jaune
    2079968 250  23.75     LED rouge
    1503144   1   2.91     lot de 100 sachets roses

    Total (sans la bande adhésive): 153.06 HT, 183.06 TTC + 12.00 port = 195.06

Options:

    1653450   1  21.37     bande electrique

Autres
======

A acheter au supermarché du coin:

    Bristol (une pochette), prix: ?
    Papier alu (un rouleau), prix: ?
    Coton, prix: ?
    Epingles de nourrice ou autres, prix: ?

A récupérer:

    Fil nu (à récupérer: fil Cat3/Cat5 de récup)

Code
====

Changements à faire:
* PWM pour la sortie
* faire moins de mesures (pour le setup -- trop long)
* sauvegarder les params dans l'EEPROM, utiliser un pin pour reset
* plusieurs sortie pour indiquer l'etat
* plusieurs sorties avec animation (pour le paquet "multi-LED"): larson scanner, PoV, ..
* monter plus vite quand le doigt est posé
* 
