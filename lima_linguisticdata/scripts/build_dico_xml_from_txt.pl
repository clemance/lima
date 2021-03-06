#!/usr/bin/perl -s
#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

open(SOURCE,"$ARGV[0]") || die ("impossible d ouvrir le fichier $ARGV[0]");
open(CIBLE,">$ARGV[1]") || die ("impossible d ouvrir le fichier $ARGV[1]");


$ligne;
$key, $type, $lemme, $norm, $categ;
$oldkey, $oldtype, $oldlemme, $oldnorm;

$nbreLignesTraitees = 1;

$lingInfos = 1;
$desaccentue = 2;
$motsATirets = 3;
$concatenation = 4;
$parDefaut = 5;

# en-tete
print CIBLE ("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
print CIBLE ("<Dictionary xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"dico4.xsd\">\n");
# fin de l'en-tete

# traitement de la premiere ligne, recuperation des infos ling
$ligne = <SOURCE>;
#chop($ligne);
chomp($ligne);
@info = split(/	/,$ligne);

$key = $info[0];	$type = $info[1];
$lemme = $info[2];	$categ = $info[3];
$norm = $info[4];	$accentue = $info[5];

print CIBLE "<k>$key</k>\n";
&trTypeDiff;

$oldkey = $key;	 $oldtype = $type;  $oldlemme = $lemme;	 $oldnorm = $norm;

# traitement des ligne suivantes 
while ($ligne = <SOURCE>){
	#chop($ligne);
	chomp($ligne);
	$nbreLignesTraitees++;
	@info = split(/	/,$ligne);	# r�up�er les infos dans la ligne
	$key = $info[0];  $type = $info[1];   $lemme = $info[2];  
	$categ = $info[3];   $norm = $info[4];   $accentue = $info[5];
	if ($key eq $oldkey) {		# si la clef courante est �ale �la clef pr��ente
	   if ($type eq $oldtype) {&trMemeType;}
	   else {&trTypeDiff;}
	}
	else {				# si la clef courante est diff�ente de la clef pr��ente
	   &Fermer($oldtype);
	   print CIBLE "<k>$key</k>
";
	   $oldtype = 0;  $oldlemme = "";
	   $oldnorm = "";  &trTypeDiff;
	}
	$oldkey = $key;	 $oldtype = $type;
	$oldlemme = $lemme;  $oldnorm = $norm;
}
&Fermer($oldtype);
print CIBLE ("</Dictionary>\n");


#----------------	Fonctions qui traite chaque cas pour les entrees differentes	-----------------#

# fonction qui traite le cas o 2 entrees ont le meme type 
sub trMemeType {
	if ($type == $lingInfos) {
		&trLingInfos($key, $lemme, $categ, $norm, $oldlemme, $oldnorm);
	}
	elsif ($type == $motsATirets) {
		&trLingInfos($key, $lemme, $categ, $norm, $oldlemme, $oldnorm);
	}
	elsif ($type == $concatenation) {
		&Fermer($type);
		&Ouvrir($type);
		&trMotsConcatenes($ligne);
	}
	elsif ($type == $desaccentue) {
		&trMotsAccentues;
	}
	elsif ($type == $parDefaut) {
		&trCatParDef;
	}
}

# fonction qui traite le cas o 2 entrees ont deux types differents
sub trTypeDiff {
	if (($oldtype == $lingInfos) || ($oldtype == $motsATirets) || ($oldtype == $concatenation)) {
		&Fermer($oldtype);
	}
	if (($type == $lingInfos) || ($type == $motsATirets) || ($type == $concatenation)) {
		&Ouvrir($type);
        }
	if ($type == $lingInfos) {
		&trLingInfos($key, $lemme, $categ, $norm, $oldlemme, $oldnorm);
	}
	elsif ($type == $motsATirets) {
		&trLingInfos($key, $lemme, $categ, $norm, $oldlemme, $oldnorm);	
	}
	elsif ($type == $concatenation) {
		&trMotsConcatenes($ligne);
	}
	elsif ($type == $desaccentue) {
		&trMotsAccentues;
	}
	elsif ($type == $parDefaut) {
		&trCatParDef;
	}
}

# fonction qui traite les ouvertures de balises pour chaque ��ent du fichier xml
sub Ouvrir {
	my($subType,$tabulations) = @_;
	&PrintTabulations($tabulations);
	if ($subType == $lingInfos) {
		print CIBLE "<i>\n";
	}
	elsif ($subType == $motsATirets) {
		print CIBLE "<ti>\n";
	}
	elsif ($subType == $concatenation) {
		print CIBLE "<c>\n";
	}
}

# fonction qui traite les fermetures de balises pour chaque ��ent du fichier xml
sub Fermer {
        my($subType,$tabulations) = @_;
	&PrintTabulations($tabulations);
        if ($subType == $lingInfos) {
                print CIBLE "</i>\n";
        }
        elsif ($subType == $motsATirets) {
               print CIBLE "</ti>\n";
        }
        elsif ($subType == $concatenation) {
               print CIBLE "</c>\n";
        }
}

sub PrintTabulations {
	my($tabulations) = @_;
	$j=0;
	while ($j<$tabulations) {
		print CIBLE "	";
		$j++;
	}
}

# fonction qui imprime les ents "mots accentues" dans le fichier source
sub trMotsAccentues {
	print CIBLE "<a>$accentue</a>\n";
}

# fonction qui imprime les ents "categories par defaut" dans le fichier source
sub trCatParDef {
	print CIBLE "<p>$categ</p>\n";
}

# fonction qui secharge du traitement des informations linguistiques relatives a chaque entree
sub trLingInfos {
	my($subKey, $subLemme, $subCateg, $subNorm, $subOldLemme, $subOldNorm,$tabulations) = @_;

	if ($subLemme eq $subOldLemme) {
		if ($subNorm eq $subOldNorm) {
			&PrintTabulations($tabulations);
			print CIBLE "	<p>$subCateg</p>\n";
		}
		else {
			if (($subNorm eq $subLemme) || ($subNorm eq "")) {
				#print CIBLE "	<n/>\n";
				&PrintTabulations($tabulations);
				print CIBLE "	<p>$subCateg</p>\n";
			}
			else {
				&PrintTabulations($tabulations);
				print CIBLE "	<l/>\n";
				&PrintTabulations($tabulations);
				print CIBLE "	<n>$subNorm</n>\n";
				&PrintTabulations($tabulations);
				print CIBLE "	<p>$subCateg</p>\n";
			}
		}
	}
	else {
		if ($subLemme eq $subKey) {
			&PrintTabulations($tabulations);
			print CIBLE "	<l/>\n";
		}
		else {
			&PrintTabulations($tabulations);
			print CIBLE "	<l>$subLemme</l>\n";
		}
		if (($subNorm eq $subLemme) || ($subNorm eq "")) {
			&PrintTabulations($tabulations);
			print CIBLE "	<n/>\n";
			&PrintTabulations($tabulations);
			print CIBLE "	<p>$subCateg</p>\n";
		}
		else { 
			&PrintTabulations($tabulations);
			print CIBLE "	<n>$subNorm</n>\n";
			&PrintTabulations($tabulations);
			print CIBLE "	<p>$subCateg</p>\n";
		}
	}
}
 
# fonction qui traite les ents "Mots concatenes" 
sub trMotsConcatenes {
	my($subLigne) = @_;
	@donnees = split(/	/,$subLigne);
	$nbreComposants = $donnees[6];
	$positionLectureCourante = 7;
	$i = 0;
	while ($i<$nbreComposants) {
		$subEntree = $donnees[$positionLectureCourante];
		$subLemme = $donnees[$positionLectureCourante+1];
		$subCateg = $donnees[$positionLectureCourante+2];
		$subNorm = $donnees[$positionLectureCourante+3];

		print CIBLE "	<cp>$subEntree</cp>\n";
		&Ouvrir($lingInfos,1);
		&trLingInfos($subEntree, $subLemme, $subCateg, $subNorm, $oldlemme, $oldnorm, 1);
		&Fermer($lingInfos,1);

		$positionLectureCourante+= 4;
		$i++;
	}
}
