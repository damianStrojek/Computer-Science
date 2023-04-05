with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO;
with Ada.Numerics.Discrete_Random;


procedure Simulation is
   Number_Of_Products: constant Integer := 6;
   Number_Of_Assemblies: constant Integer := 3;
   Number_Of_Consumers: constant Integer := 2;

   subtype Product_Type is Integer range 1 .. Number_Of_Products;
   subtype Assembly_Type is Integer range 1 .. Number_Of_Assemblies;
   subtype Consumer_Type is Integer range 1 .. Number_Of_Consumers;

   Product_Name: constant array (Product_Type) of String(1 .. 13) := ("procesor     ", "GPU          ", "plyta glowna ", "zasilacz     ", "pamiec RAM   ", "pamiec glowna");
   Assembly_Name: constant array (Assembly_Type) of String(1 .. 6) := ("laptop", "PC    ", "ploter");

   package Random_Assembly is new Ada.Numerics.Discrete_Random(Assembly_Type);

   -- Producent produkuje konkretny produkt
   task type Producer is
      -- Trzeba dac producentowi identyfikator np typ produktu
      entry Start(Product: in Product_Type; Production_Time: in Integer);
   end Producer;

   -- Konsument dostaje kilka produktow z bufora
   task type Consumer is
      -- Konsument dostaje identyfikator (typ konsumenta)
      entry Start(Consumer_Number: in Consumer_Type; Consumption_Time: in Integer);
   end Consumer;

   -- W buforze produkty sa laczone w assembly
   task type Buffer is
      -- Zaakceptuj produkt do magazynu jezeli jest na niego miejsce
      entry Take(Product: in Product_Type; Number: in Integer);
      -- Dostarcz assembly jezeli jest wystarczajaco produktow zeby go stworzyc
      entry Deliver(Assembly: in Assembly_Type; Number: out Integer);
   end Buffer;

   P: array ( 1 .. Number_Of_Products ) of Producer;
   K: array ( 1 .. Number_Of_Consumers ) of Consumer;
   B: Buffer;

   task body Producer is
      subtype Production_Time_Range is Integer range 3 .. 6;
      package Random_Production is new Ada.Numerics.Discrete_Random(Production_Time_Range);
      -- Generator liczb losowych
      G: Random_Production.Generator;
      Product_Type_Number: Integer;
      Product_Number: Integer;
      Production: Integer;

   begin
      accept Start(Product: in Product_Type; Production_Time: in Integer) do
         Random_Production.Reset(G);
         Product_Number := 1;
         Product_Type_Number := Product;
         Production := Production_Time;
      end Start;

      Put_Line("Rozpoczeto produkcje " & Product_Name(Product_Type_Number));

      loop
         -- Symuluj produkcje
         delay Duration(Random_Production.Random(G));
         Put_Line("Wyprodukowano produkt " & Product_Name(Product_Type_Number) & " o numerze "  & Integer'Image(Product_Number));
         -- Zaakceptuj do magazynu
         loop
            select
               B.Take(Product_Type_Number, Product_Number);
               Product_Number := Product_Number + 1;
               exit;
            else
               Put_Line("Magazyn jest zastawiony kurierami DHL. Poczekaj 3 sekundy.");
               delay Duration(3.0);
            end select;
         end loop;
      end loop;

   end Producer;

   task body Consumer is
      subtype Consumption_Time_Range is Integer range 4 .. 8;
      package Random_Consumption is new Ada.Numerics.Discrete_Random(Consumption_Time_Range);
      G: Random_Consumption.Generator;
      G2: Random_Assembly.Generator;
      Consumer_Nb: Consumer_Type;
      Assembly_Number: Integer;
      Consumption: Integer;
      Assembly_Type: Integer;
      Consumer_Name: constant array (1 .. Number_Of_Consumers) of String(1 .. 10) := ("RTVEUROAGD", "MediaMarkt");

   begin
      accept Start(Consumer_Number: in Consumer_Type;
                   Consumption_Time: in Integer) do
         Random_Consumption.Reset(G);	--  ustaw generator
         Random_Assembly.Reset(G2);	--  tez
         Consumer_Nb := Consumer_Number;
         Consumption := Consumption_Time;
      end Start;

      Put_Line("Otworzyl sie " & Consumer_Name(Consumer_Nb));

      loop
         -- Symulacja czasu
         delay Duration(Random_Consumption.Random(G));
         Assembly_Type := Random_Assembly.Random(G2);
         -- Wez jakies assembly do konsumpcji
         B.Deliver(Assembly_Type, Assembly_Number);
         if Assembly_Number = 0 then
            Put_Line(Consumer_Name(Consumer_Nb) & " zamowil " & Assembly_Name(Assembly_Type) & " ale nie mogl go sprzedac.");
         else
            Put_Line(Consumer_Name(Consumer_Nb) & " sprzedal " & Assembly_Name(Assembly_Type) & " o numerze " & Integer'Image(Assembly_Number));
         end if;
      end loop;
   end Consumer;

   task body Buffer is
      Storage_Capacity: constant Integer := 30;
      type Storage_type is array (Product_Type) of Integer;
      Storage: Storage_type := (0, 0, 0, 0, 0, 0);
      Assembly_Content: array(Assembly_Type, Product_Type) of Integer := ((1, 0, 1, 1, 0, 1), (1, 1, 1, 1, 2, 1), (1, 1, 0, 0, 0, 1));
      Max_Assembly_Content: array(Product_Type) of Integer;
      Assembly_Number: array(Assembly_Type) of Integer := (1, 1, 1);
      In_Storage: Integer := 0;
      iloscNieudanychDostaw : Integer := 0;
      iloscPamieciRAM : Integer := 0;

      procedure Setup_Variables is
      begin
         for W in Product_Type loop
            Max_Assembly_Content(W) := 0;
            for Z in Assembly_Type loop
               if Assembly_Content(Z, W) > Max_Assembly_Content(W) then
                  Max_Assembly_Content(W) := Assembly_Content(Z, W);
               end if;
            end loop;
         end loop;
      end Setup_Variables;

      -- Wyczysc miejsce w magazynie
      function Can_Accept(Product: Product_Type) return Boolean is Free: Integer;
         -- Ile jest produktow do produkcji danego assembly
         Lacking: array(Product_Type) of Integer;
         -- Jak duzo miejsca potrzeba w megazynie zeby wyprodukowac cos tam
         Lacking_room: Integer;
         MP: Boolean;

      begin
         if In_Storage >= Storage_Capacity then
            return False;
         end if;
         -- Jezeli jest wolne miejse w magazynie :
         Free := Storage_Capacity - In_Storage;
         MP := True;

         for W in Product_Type loop
            if Storage(W) < Max_Assembly_Content(W) then
               MP := False;
            end if;
         end loop;

         if MP then
            return True;
            -- stworz dane assembly
         end if;

         if Integer'Max(0, Max_Assembly_Content(Product) - Storage(Product)) > 0 then
            -- dokladnie tego produktu brakuje
            return True;
         end if;

         -- dodaj konkretny produkt
         Lacking_room := 1;

         for W in Product_Type loop
            Lacking(W) := Integer'Max(0, Max_Assembly_Content(W) - Storage(W));
            Lacking_room := Lacking_room + Lacking(W);
         end loop;

         if Free >= Lacking_room then
            -- Jest wystarczajaco miejsca na konkretny zestaw
            return True;
         else
            -- Nie ma miejsca na zestaw
            return False;
         end if;
      end Can_Accept;

      -- czy mozna dostarczyc
      function Can_Deliver(Assembly: Assembly_Type) return Boolean is
      begin
         for W in Product_Type loop
            if Storage(W) < Assembly_Content(Assembly, W) then
               return False;
            end if;
         end loop;
         return True;
      end Can_Deliver;

      -- wyprintowanie magazynu
      procedure Storage_Contents is
      begin
         for W in Product_Type loop
            Put_Line("Zawartosc magazynu : " & Integer'Image(Storage(W)) & " " & Product_Name(W));
         end loop;
      end Storage_Contents;

      subtype Production_Quantity is Integer range 1 .. 2;
      package Random_Quantity is new Ada.Numerics.Discrete_Random(Production_Quantity);
      Quantity: Random_Quantity.Generator;
      QuantityToAdd: Integer;
      
      
      -- rozpoczecie bufora
   begin
      Put_Line("Bufor wystartowal.");
      Setup_Variables;
      
      loop
         select
            accept Deliver(Assembly: in Assembly_Type; Number: out Integer) do
               if Can_Deliver(Assembly) then
                  Put_Line("Dostarczono wyrob " & Assembly_Name(Assembly) & " o numerze " & Integer'Image(Assembly_Number(Assembly)));

                  for W in Product_Type loop
                     Storage(W) := Storage(W) - Assembly_Content(Assembly, W);
                     In_Storage := In_Storage - Assembly_Content(Assembly, W);
                  end loop;

                  Number := Assembly_Number(Assembly);
                  Assembly_Number(Assembly) := Assembly_Number(Assembly) + 1;
               else
                  -- kwestia przepelnienia sie magazynu - jezeli nie dostarczylismy za duzo zamowien to usunie nam zawartosc magazynu
                  iloscNieudanychDostaw := iloscNieudanychDostaw + 1;
                  if iloscNieudanychDostaw >= 8 then
                     iloscNieudanychDostaw := 0;
                     New_Line;
                     Put_Line("Nie dostarczylismy bardzo duzej ilosci zamowien! Nasi klienci sie zirytowali i zburzyli nasz magazyn.");
                     New_Line;
                     for W in Product_Type loop
                        Storage(W) := 0;
                     end loop;
                     In_Storage := 0;
                  else
                     Put_Line("Brakuje produktow na wyrob " & Assembly_Name(Assembly));
                     Number := 0;
                  end if;
               end if;
               Storage_Contents;	
            end Deliver;
         or
            accept Take(Product: in Product_Type; Number: in Integer) do
               QuantityToAdd := Random_Quantity.Random(Quantity);
               if Can_Accept(Product) then
                  Put_Line("Zaakceptowano produkt " & Product_Name(Product) & " o numerze " & Integer'Image(Number) & " w ilosci " & Integer'Image(QuantityToAdd));
                  Storage(Product) := Storage(Product) + QuantityToAdd;
                  In_Storage := In_Storage + QuantityToAdd;

                  -- kwestia zaglodzenia magazynu
                  for W in Product_Type loop
                     if Storage(W) >= 7 then
                        Storage(W) := Storage(W) / 2;
                        Put_Line("Zostalo wyprodukowane za duzo " & Product_Name(W) & ". Polowa zostala rozdana studentom.");
                        In_Storage := In_Storage - Storage(W);
                     end if;
                  end loop;
               else
                  Put_Line("Odrzucono produkty " & Product_Name(Product) & " o numerze " & Integer'Image(Number) & " w ilosci " & Integer'Image(QuantityToAdd));
               end if;
            end Take;
         end select;
      end loop;
   end Buffer;

   -- Rozpoczecie symulacji
begin
   for I in 1 .. Number_Of_Products loop
      P(I).Start(I, 10);
   end loop;
   for J in 1 .. Number_Of_Consumers loop
      K(J).Start(J,12);
   end loop;
end Simulation;
