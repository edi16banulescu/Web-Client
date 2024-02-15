Am implementat un client care interactioneaza cu un REST API si trimite cereri de tip HTTP de tip POST, GET, DELETE.
Clientul accepta comenzi de la tastatura si trimite in functie, de comanda, un request de tipul corespunzator.
Am folosit functiile din laborator pentru creearea request-ului si am creeat eu functie pentru creearea unui json
corespunzator folosind biblioteca PARSON. Fiind vorba despre un client HTTP, deschid conexiunea cu serverul si o inchid
inainte si dupa fiecare request. Am folosit functia de la laborator pentru a extrage din raspunsul primit de la server, iar
mai departe am interpretat json-ul cu functiile din biblioteca PARSON pentru a putea printa mesajul de eroare sau de succes.
Clientul poate primi de la tastatura comenzile:
- login : pentru a te loga in contul tau, iar serverul intoarce un cookie de sesiune sau un mesaj de eroare cum ca nu username-ul sau
            parola sunt gresite, de asemenea, acesta nu trimite cererea daca username-ul sau parola contin spatii (conform cerintei).
- register : pentru a te inregistra in aplicatie, iar serverul intoarce un mesaj de succes sau de eroare cum ca username-ul este deja
            folosit sau ca username-ul sau parola contin spatii (conform cerintei).
- enter_libray : pentru a intra in biblioteca, iar serverul intoarce in caz de succes un token JWT pe care il vom folosi mai departe
            pentru accesul in biblioteca, iar in caz de eroare, un mesaj de eroare.
- get_books : pentru a primi lista de carti din biblioteca, iar serverul intoarce in caz de succes lista de carti, iar in caz de eroare
            un mesaj de eroare. Am folosit biblioteca parson pentru a creea un array de json uri si a le printa mai departe.
- get_book : pentru a primi informatii despre o carte din biblioteca, iar serverul intoarce in caz de succes informatiile despre carte,
            iar in caz de eroare un mesaj de eroare de acces sau cum ca cartea nu exista.(Am verificat ca id-ul introdus sa fie un numar)
- add_book : pentru a adauga o carte in biblioteca, iar serverul intoarce in caz de succes un mesaj de succes, iar in caz de eroare un
            mesaj de eroare de acces sau cum ca datele sunt invalide. (Am verificat ca string-urile sa nu fie nule si page-count sa fie un numar)
- delete_book : se efectueaza o cerere de tip DELETE pentru a sterge o carte din biblioteca, iar serverul intoarce in caz de succes un
                un mesaj de succes, iar in caz de eroare un mesaj de eroare de acces sau cum ca cartea nu exista. (Am verificat ca id-ul introdus sa fie un numar)
- logout : se efectueaza o cerere de tip GET si delogheaza userul curent, iar cookie-ul de sesiune si tokenul jwt este sters din client.
- exit : se inchide clientul.

Pentru orice alta comanda, clientul va printa un mesaj de eroare.
Am incercat sa acopar toate edge-case-urile, daca nu am reusit I'm sorry :) .