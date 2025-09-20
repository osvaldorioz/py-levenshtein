| Parámetro | Opciones Posibles | Descripción | Impacto en el Algoritmo | Ejemplo de Uso |
|-----------|-------------------|-------------|--------------------------|---------------|
| **s1** | Cualquier cadena de texto (str) | Cadena fuente para comparar. Obligatorio. | Base para calcular ediciones (inserciones, eliminaciones, sustituciones). | "The quick brown fox" |
| **s2** | Cualquier cadena de texto (str) | Cadena objetivo para comparar. Obligatorio. | Base para calcular ediciones. | "The fast brown dog" |
| **discount_from** | - Numérico: Cualquier entero ≥0 como string (e.g., "0", "1", "5")<br>- Especial: "coda" | Posición desde la cual aplicar descuentos. Si numérico, aplica descuento después de esa posición en ambas cadenas. Si "coda", calcula dinámicamente la posición de la "coda" basada en vocales. | Reduce el costo de ediciones (inserciones/eliminaciones/sustituciones/transposiciones) después de la posición. Para "coda", ignora vowels si no es "coda". | "3" (descuento desde posición 3); "coda" (descuento desde coda silábica) |
| **discount_func** | - "log"<br>- "exp" | Función para calcular el descuento en costos de ediciones. | - "log": Costo = 1 / (log(1 + d/5) + 1), donde d = operaciones después de discount_from (descuento suave).<br>- "exp": Costo = 1 / (d + 1)^0.2 (descuento más agresivo para d grandes). | "log" para descuentos graduales; "exp" para cadenas largas con ediciones tardías |
| **mode** | - "lev"<br>- "osa" | Modo del algoritmo Levenshtein. | - "lev": Solo inserciones, eliminaciones, sustituciones (distancia clásica).<br>- "osa": Incluye transposiciones adyacentes (e.g., "ab" → "ba" cuesta 1 en lugar de 2). | "lev" para comparaciones estrictas; "osa" para tolerar reordenamientos locales |
| **vowels** | Cualquier cadena de vocales (str, e.g., "aeiou", "áéíóú") | Conjunto de vocales para calcular la "coda" cuando discount_from="coda". | Solo usado si discount_from="coda": Encuentra la primera vocal y el siguiente no-vocal para definir la posición de descuento. Ignorado en otros casos. | "aeiou" para inglés/español; "aeiouáéíóú" para acentos |

### Combinaciones Posibles
Dado que `s1` y `s2` son arbitrarias (infinitas), y `vowels` es relevante solo para `discount_from="coda"`, las combinaciones clave son entre `discount_from` (2 tipos), `discount_func` (2), `mode` (2), y `vowels` (variable, pero fijo en ejemplos). Hay **8 combinaciones base** (2×2×2, ignorando variaciones numéricas de discount_from y vowels). A continuación, una tabla con todas las combinaciones base, asumiendo `discount_from` como "numérico" o "coda", `vowels="aeiou"` fijo para "coda", y un ejemplo de JSON.

| # | discount_from | discount_func | mode | vowels | Descripción de Combinación | JSON de Ejemplo (para s1="cat", s2="hat") |
|---|---------------|---------------|------|--------|----------------------------|-------------------------------------------|
| 1 | Numérico (e.g., "1") | log | lev | aeiuo (ignorado) | Levenshtein clásico con descuento logarítmico desde posición fija. | `{"s1":"cat","s2":"hat","discount_from":"1","discount_func":"log","mode":"lev","vowels":"aeiou"}` |
| 2 | Numérico (e.g., "1") | log | osa | aeiuo (ignorado) | OSA con descuento logarítmico desde posición fija; útil para transposiciones. | `{"s1":"cat","s2":"hat","discount_from":"1","discount_func":"log","mode":"osa","vowels":"aeiou"}` |
| 3 | Numérico (e.g., "1") | exp | lev | aeiuo (ignorado) | Levenshtein con descuento exponencial desde posición fija; agresivo en ediciones tardías. | `{"s1":"cat","s2":"hat","discount_from":"1","discount_func":"exp","mode":"lev","vowels":"aeiou"}` |
| 4 | Numérico (e.g., "1") | exp | osa | aeiuo (ignorado) | OSA con descuento exponencial; ideal para textos con reordenamientos y ediciones lejanas. | `{"s1":"cat","s2":"hat","discount_from":"1","discount_func":"exp","mode":"osa","vowels":"aeiou"}` |
| 5 | coda | log | lev | aeiuo | Levenshtein con descuento logarítmico desde coda silábica; contextual para fonética. | `{"s1":"cat","s2":"hat","discount_from":"coda","discount_func":"log","mode":"lev","vowels":"aeiou"}` |
| 6 | coda | log | osa | aeiuo | OSA con descuento logarítmico desde coda; tolera transposiciones en contextos silábicos. | `{"s1":"cat","s2":"hat","discount_from":"coda","discount_func":"log","mode":"osa","vowels":"aeiou"}` |
| 7 | coda | exp | lev | aeiuo | Levenshtein con descuento exponencial desde coda; fuerte descuento en finales de palabras. | `{"s1":"cat","s2":"hat","discount_from":"coda","discount_func":"exp","mode":"lev","vowels":"aeiou"}` |
| 8 | coda | exp | osa | aeiuo | OSA con descuento exponencial desde coda; máxima flexibilidad para textos fonéticos con reordenes. | `{"s1":"cat","s2":"hat","discount_from":"coda","discount_func":"exp","mode":"osa","vowels":"aeiou"}` |

### Alcance del Microservicio
- **Total de Combinaciones**: Infinitas debido a variaciones en `s1/s2`, valores numéricos de `discount_from` (e.g., "0" a "100+"), y `vowels` (cualquier str). Las 8 base cubren los modos lógicos.
- **Limitaciones**: No valida inputs (e.g., discount_from no numérico falla en stoi); asume ASCII para tolower en coda. Rendimiento O(m*n) para cadenas de longitud m/n (eficiente hasta ~10k chars).
- **Uso Recomendado**: Para similitud de texto en NLP (e.g., corrección ortográfica, matching fonético). Prueba con oraciones largas para ver descuentos en acción.
- **Extensibilidad**: Fácil agregar más funcs/modos vía ports en C++ (arquitectura hexagonal). 

Esta tabla resume el alcance: 2 funcs × 2 modos × 2 tipos de discount_from = 8 configuraciones principales, con `vowels` como modificador para "coda".### Alcance del Microservicio: Opciones y Combinaciones del Endpoint `/distance`

El microservicio FastAPI expone un endpoint POST `/distance` que calcula la distancia Levenshtein descontada entre dos cadenas (`s1` y `s2`). Los parámetros obligatorios son `s1` y `s2` (cadenas de texto arbitrarias, sin longitud máxima definida en el código, pero el algoritmo es eficiente para cadenas de hasta miles de caracteres). Los parámetros opcionales tienen valores por defecto y validaciones implícitas en el código C++ (e.g., `discount_from` como entero o "coda", `discount_func` como "log" o "exp", etc.). No hay validaciones estrictas en FastAPI, pero el código maneja errores gracefully (e.g., stoi para enteros).

Para entender el **alcance completo**, a continuación se detalla:
1. **Opciones individuales** para cada parámetro opcional.
2. **Todas las combinaciones posibles** en una tabla (generada exhaustivamente). He seleccionado opciones representativas y válidas basadas en el código:
   - `discount_from`: Números enteros positivos (ejemplos: "1", "2", "3", "4") o "coda" (4 números de ejemplo × 1 = 5 opciones).
   - `discount_func`: "log" o "exp" (2 opciones).
   - `mode`: "lev" (Levenshtein estándar) o "osa" (con transposiciones) (2 opciones).
   - `vowels`: Cadenas de vocales comunes (e.g., "aeiou" para inglés básico, "aeiouy" incluyendo 'y', "aáeéiíoóuú" para español/accentuados) (3 opciones).
   
   **Total de combinaciones**: 5 × 2 × 2 × 3 = 60. Estas cubren el 100% del espacio paramétrico posible (cualquier entero válido en `discount_from` funciona, y `vowels` es flexible como string).

#### 1. Opciones por Parámetro
| Parámetro       | Opciones Válidas/Posibles | Descripción | Valor por Defecto |
|-----------------|---------------------------|-------------|-------------------|
| **s1**         | Cualquier string (e.g., "cat", "The quick brown fox") | Cadena fuente para comparación. | N/A (obligatorio) |
| **s2**         | Cualquier string (e.g., "hat", "The fast brown dog") | Cadena objetivo para comparación. | N/A (obligatorio) |
| **discount_from** | String: entero positivo (e.g., "1", "5", "10") o "coda" | Posición desde donde aplicar descuento (entero: fijo; "coda": dinámico basado en vocales). | "1" |
| **discount_func** | "log" o "exp" | Función de descuento: "log" (logarítmica: 1/(log(1 + d/5) + 1)); "exp" (exponencial: 1/(d+1)^0.2). | "log" |
| **mode**       | "lev" o "osa" | Algoritmo: "lev" (solo inserción/eliminación/sustitución); "osa" (incluye transposiciones). | "lev" |
| **vowels**     | Cualquier string de caracteres (e.g., "aeiou", "aeiouy", "aáeéiíoóuú") | Vocales para calcular "coda" (solo usado si discount_from="coda"). Ignorado en otros casos. | "aeiou" |

**Notas sobre el alcance**:
- **Flexibilidad**: `s1` y `s2` son ilimitados (cadenas vacías devuelven distancia basada en la otra cadena). `discount_from` acepta cualquier entero ≥1 (valores ≤0 se tratan como sin descuento). `vowels` es case-insensitive (el código usa tolower).
- **Comportamiento edge-case**: Si s1 == s2, distancia = 0. Si una cadena es vacía, distancia = suma de costos descontados de la otra. No hay límites en combinaciones; todas son válidas.
- **Rendimiento**: O(m*n) donde m/n son longitudes de s1/s2; ideal para oraciones (e.g., 100 chars: <1ms).
- **Endpoint adicional**: GET `/health` (sin parámetros, devuelve {"status": "ok"}).

#### 2. Tabla de Todas las Combinaciones Posibles
La tabla muestra las 60 combinaciones exhaustivas. Cada fila representa una configuración única que se puede probar en el JSON de solicitud POST. 
Ejemplo de uso usando la primera linea de la tabla: 

{
  "s1": "The big dwarf only jumps",
  "s2": "The dwarf big only hops",
  "discount_from": "1",
  "discount_func": "log",
  "mode": "lev",
  "vowels": "aeiou"
}

| discount_from | discount_func | mode | vowels          |
|---------------|---------------|------|-----------------|
| 1             | log           | lev  | aeiou           |
| 1             | log           | lev  | aeiouy          |
| 1             | log           | lev  | aáeéiíoóuú      |
| 1             | log           | osa  | aeiou           |
| 1             | log           | osa  | aeiouy          |
| 1             | log           | osa  | aáeéiíoóuú      |
| 1             | exp           | lev  | aeiou           |
| 1             | exp           | lev  | aeiouy          |
| 1             | exp           | lev  | aáeéiíoóuú      |
| 1             | exp           | osa  | aeiou           |
| 1             | exp           | osa  | aeiouy          |
| 1             | exp           | osa  | aáeéiíoóuú      |
| 2             | log           | lev  | aeiou           |
| 2             | log           | lev  | aeiouy          |
| 2             | log           | lev  | aáeéiíoóuú      |
| 2             | log           | osa  | aeiou           |
| 2             | log           | osa  | aeiouy          |
| 2             | log           | osa  | aáeéiíoóuú      |
| 2             | exp           | lev  | aeiou           |
| 2             | exp           | lev  | aeiouy          |
| 2             | exp           | lev  | aáeéiíoóuú      |
| 2             | exp           | osa  | aeiou           |
| 2             | exp           | osa  | aeiouy          |
| 2             | exp           | osa  | aáeéiíoóuú      |
| 3             | log           | lev  | aeiou           |
| 3             | log           | lev  | aeiouy          |
| 3             | log           | lev  | aáeéiíoóuú      |
| 3             | log           | osa  | aeiou           |
| 3             | log           | osa  | aeiouy          |
| 3             | log           | osa  | aáeéiíoóuú      |
| 3             | exp           | lev  | aeiou           |
| 3             | exp           | lev  | aeiouy          |
| 3             | exp           | lev  | aáeéiíoóuú      |
| 3             | exp           | osa  | aeiou           |
| 3             | exp           | osa  | aeiouy          |
| 3             | exp           | osa  | aáeéiíoóuú      |
| 4             | log           | lev  | aeiou           |
| 4             | log           | lev  | aeiouy          |
| 4             | log           | lev  | aáeéiíoóuú      |
| 4             | log           | osa  | aeiou           |
| 4             | log           | osa  | aeiouy          |
| 4             | log           | osa  | aáeéiíoóuú      |
| 4             | exp           | lev  | aeiou           |
| 4             | exp           | lev  | aeiouy          |
| 4             | exp           | lev  | aáeéiíoóuú      |
| 4             | exp           | osa  | aeiou           |
| 4             | exp           | osa  | aeiouy          |
| 4             | exp           | osa  | aáeéiíoóuú      |
| coda          | log           | lev  | aeiou           |
| coda          | log           | lev  | aeiouy          |
| coda          | log           | lev  | aáeéiíoóuú      |
| coda          | log           | osa  | aeiou           |
| coda          | log           | osa  | aeiouy          |
| coda          | log           | osa  | aáeéiíoóuú      |
| coda          | exp           | lev  | aeiou           |
| coda          | exp           | lev  | aeiouy          |
| coda          | exp           | lev  | aáeéiíoóuú      |
| coda          | exp           | osa  | aeiou           |
| coda          | exp           | osa  | aeiouy          |
| coda          | exp           | osa  | aáeéiíoóuú      |

**Notas sobre la tabla**:
- **Escalabilidad**: Para más valores en `discount_from` (e.g., "5" a "100"), el patrón se repite (multiplica por 2×2×3=12 combinaciones por valor). `vowels` puede ser cualquier string, expandiendo infinitamente.
- **Pruebas recomendadas**: Usa las combinaciones con `s1` y `s2` de ejemplos previos (e.g., "The big dwarf only jumps" vs "The dwarf big only hops") para ver variaciones en la distancia (e.g., "osa" reduce distancias en transposiciones; "coda" + vocales acentuadas afecta cálculos en español).
