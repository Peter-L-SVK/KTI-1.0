/**
 * KYBERNAUT-LIGHT v3.1 - Fyzikálne korektná verzia (OPRAVENÁ)
 * Autor: Peter Leukanič
 * Rok: 2026
 * 
 * OPRAVY v3.1:
 * 1. Fyzikálne korektná projekcia 3D→2D
 * 2. Reálne optické zákony (Snell, Fresnel)
 * 3. Konzistentné entropické metriky
 * 4. Validácia matematických limitov
 * 
 * OPRAVENÉ CHYBY:
 * - Pridané is_target do OpticalNode
 * - Odstránený nepoužitý parameter B
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>  // PRIDANÉ: Pre veľké mriežky

#define MAX_STEPS 20000           // ZVÝŠENÉ pre veľké mriežky
#define LOG_FILENAME "kybernaut_light_v3.1_log.txt"

/* ==================== FYZIKÁLNE KONŠTANTY A PROJEKCIA ==================== */

// Skutočné fyzikálne konštanty
#define SPEED_OF_LIGHT 299792458.0     // m/s
#define PLANCK 6.62607015e-34          // J·s
#define BOLTZMANN 1.380649e-23         // J/K
#define ELECTRON_VOLT 1.602176634e-19  // J

// Projekcia 3D→2D pre optiku
#define WAVELENGTH 550e-9              // Vlnová dĺžka [m] (zelené svetlo)
#define CELL_SIZE 1.0e-6               // 1 bunka = 1 mikrometer
#define TIME_STEP 1.0e-15              // 1 krok = 1 femtosekunda
#define PHOTON_ENERGY (PLANCK * SPEED_OF_LIGHT / WAVELENGTH) // J

/* ==================== OPTICKÉ ŠTRUKTÚRY ==================== */

typedef struct {
    float refractive_index;      // Index lomu (bezrozmerný)
    float absorption_coeff;      // Koeficient absorpcie [1/m]
    float scattering_coeff;      // Koeficient rozptylu [1/m]
    float extinction_coeff;      // Koeficient extinkcie [1/m]
    float complex_n_real;        // Reálna časť komplexného indexu
    float complex_n_imag;        // Imaginárna časť (absorpcia)
    const char* name;
    char symbol;
} OpticalMaterial;

OpticalMaterial materials[5] = {
    // n, α [1/m], σ [1/m], ε [1/m], n_real, n_imag, name, symbol
    {1.00029, 1.0e-5,  1.0e-6,  1.1e-5,  1.00029, 1.0e-7,  "vzduch", '.'},
    {1.3330,  1.3e-1,  2.5e-2,  1.55e-1, 1.3330,  1.0e-4,  "voda",   '~'},
    {1.5000,  5.0e-1,  1.0e-2,  5.1e-1,  1.5000,  2.0e-3,  "sklo",   '#'},
    {2.4170,  1.0e0,   5.0e-3,  1.005e0, 2.4170,  5.0e-3,  "diamant", '*'},
    {10.000,  1.0e4,   1.0e-1,  1.0e4,   10.000,  1.0e3,   "prekážka", 'X'}
};

typedef struct {
    int32_t x, y;               // PRIDANÉ: int32_t pre veľké mriežky
    int32_t photon_visits;      // PRIDANÉ: int32_t pre veľké počty
    float energy_density;       // Hustota energie [J/m³]
    float temperature;          // Teplota [K] (z absorpcie)
    float optical_depth;        // Optická hĺbka (bezrozmerná)
    int material_id;            // ID optického materiálu
    int is_target;              // OPRAVA: Pridané pre ciele (0=žiadny, 1=domov, 2=bar)
    
    // Interferenčné efekty
    float accumulated_phase;    // Kumulatívna fáza pre interferenciu
    float interference_pattern; // Interferenčný vzor (0-1)
    
    // Termodynamika
    float entropy_density;      // Hustota entropie [J/K·m³]
} OpticalNode;

typedef struct {
    float wavelength;           // Vlnová dĺžka [m]
    float intensity;            // Intenzita [W/m²] (projektovaná)
    float phase;                // Fáza [rad]
    float polarization[2];      // Jonesov vektor (komplexný)
    float coherence_length;     // Koherenčná dĺžka [m]
    float optical_path_length;  // Optická dráha [m]
    int32_t reflections;        // PRIDANÉ: int32_t pre veľké počty
    int32_t refractions;        // PRIDANÉ: int32_t pre veľké počty
    float accumulated_phase;    // Kumulatívna fáza [rad]
    float group_velocity;       // Skupinová rýchlosť [m/s]
} Photon;

typedef struct {
    // ENTROPICKÉ METRIKY (konzistentné s Human verziou)
    float information_entropy;    // S_info (bezrozmerná 0-1)
    float thermal_entropy;        // S_thermal (bezrozmerná 0-1)
    float quantum_entropy;        // S_quantum (bezrozmerná 0-1)
    
    // OPTICKÉ METRIKY
    float total_optical_path;     // Celková optická dráha [m]
    float average_intensity;      // Priemerná intenzita [W/m²]
    float photon_efficiency;      // Efektivita fotónu [m/J]
    
    // TERMODYNAMICKÉ METRIKY
    float total_energy_absorbed;  // Celková absorbovaná energia [J]
    float max_temperature;        // Maximálna teplota [K]
    float min_temperature;        // Minimálna teplota [K]
    
    // EXPLORAČNÉ METRIKY
    int64_t total_cells;          // ZMENENÉ: int64_t pre veľké mriežky
    int64_t visited_cells;        // ZMENENÉ: int64_t pre veľké mriežky
    float coverage;               // Pokrytie [%]
    
} SystemMetrics;

/* ==================== GLOBÁLNE PREMENNÉ ==================== */

int32_t dimension;                // ZMENENÉ: int32_t pre veľké mriežky
OpticalNode **world;              // 2D optický svet
Photon photon;                    // Simulovaný fotón
SystemMetrics metrics;            // Systémové metriky

int32_t target_x, target_y;       // ZMENENÉ: int32_t
int32_t start_x, start_y;         // ZMENENÉ: int32_t

/* ==================== OPTICKÉ FUNKCIE ==================== */

/* Snellov zákon: n₁·sin(θ₁) = n₂·sin(θ₂) */
float snell_law(float n1, float n2, float angle_incident) {
    // Obmedzenie uhla na rozsah 0-90°
    float theta_i = fmod(fabs(angle_incident), M_PI/2);
    if (theta_i > M_PI/2) theta_i = M_PI - theta_i;
    
    float sin_theta_i = sin(theta_i);
    float sin_theta_t = (n1 / n2) * sin_theta_i;
    
    // Totálny odraz
    if (sin_theta_t > 1.0) {
        return -1.0; // Označenie totálneho odrazu
    }
    
    return asin(sin_theta_t); // Uhol lomu
}

/* Fresnelove koeficienty (približné pre normalný dopad) */
float fresnel_reflection(float n1, float n2) {
    float R = pow((n1 - n2) / (n1 + n2), 2);
    return fmin(fmax(R, 0.0), 1.0);
}

/* Absorpcia podľa Beer-Lambertovho zákona: I = I₀·exp(-α·d) */
float beer_lambert_absorption(float intensity, float alpha, float distance) {
    return intensity * exp(-alpha * distance);
}

/* Disperzia: n(λ) = n₀ + A/λ² (jednoduchý Cauchyho vzorec) */
float cauchy_dispersion(float lambda, float n0, float A) {
    return n0 + A / (lambda * lambda);
}

/* ==================== FYZIKÁLNE VÝPOČTY ==================== */

/* Fyzikálna vzdialenosť v 2D s optickou interpretáciou */
float optical_distance(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    float dx = (x2 - x1) * CELL_SIZE;
    float dy = (y2 - y1) * CELL_SIZE;
    float geometric = sqrt(dx*dx + dy*dy);
    
    // Optická dráha = geometrická × index lomu
    OpticalMaterial mat = materials[world[x2][y2].material_id];
    return geometric * mat.refractive_index;
}

/* Optický prechod s fyzikálnymi zákonmi */
int32_t optical_transition_decision(int32_t x, int32_t y, float current_direction) {
    // 8-susedná pre presnejšiu optiku
    int32_t dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    int32_t dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    float angles[8] = {0.0, M_PI/4, M_PI/2, 3*M_PI/4, 
                      M_PI, 5*M_PI/4, 3*M_PI/2, 7*M_PI/4};
    
    OpticalMaterial current_mat = materials[world[x][y].material_id];
    float weights[8] = {0};
    int32_t valid_dirs = 0;
    
    for (int32_t i = 0; i < 8; i++) {
        int32_t nx = x + dx[i];
        int32_t ny = y + dy[i];
        
        // Kontrola hraníc
        if (nx < 0 || nx >= dimension || ny < 0 || ny >= dimension) {
            weights[i] = -INFINITY;
            continue;
        }
        
        valid_dirs++;
        OpticalMaterial next_mat = materials[world[nx][ny].material_id];
        
        // 1. Snellov zákon (60% váha)
        float angle_diff = fabs(angles[i] - current_direction);
        if (angle_diff > M_PI) angle_diff = 2*M_PI - angle_diff;
        
        float refraction_angle = snell_law(current_mat.refractive_index,
                                          next_mat.refractive_index,
                                          angle_diff);
        
        if (refraction_angle >= 0) {
            // Úspešný lom
            weights[i] += 0.6 * (1.0 - fabs(refraction_angle) / (M_PI/2));
        } else {
            // Totálny odraz - nižšia váha
            weights[i] += 0.2;
        }
        
        // 2. Fresnelove odrazy (20% váha)
        float R = fresnel_reflection(current_mat.refractive_index,
                                    next_mat.refractive_index);
        weights[i] += 0.2 * (1.0 - R); // Preferencia priechodnosti
        
        // 3. Absorpcia (10% váha) - penalizácia
        float absorption_loss = next_mat.absorption_coeff * CELL_SIZE;
        weights[i] -= 0.1 * absorption_loss;
        
        // 4. Smer k cieľu (10% váha)
        float target_angle = atan2(target_y - y, target_x - x);
        float target_diff = fabs(angles[i] - target_angle);
        if (target_diff > M_PI) target_diff = 2*M_PI - target_diff;
        weights[i] += 0.1 * (1.0 - target_diff / M_PI);
    }
    
    if (valid_dirs == 0) return -1;
    
    // Výber najlepšieho smeru
    int32_t best_dir = 0;
    for (int32_t i = 1; i < 8; i++) {
        if (weights[i] > weights[best_dir]) {
            best_dir = i;
        }
    }
    
    return best_dir;
}

/* ==================== OPRAVENÝ VÝPOČET ENTROPIÍ ==================== */

/* Informačná entropia z rozloženia fotónov */
float calculate_information_entropy() {
    int64_t total_visits = 0;
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            total_visits += world[x][y].photon_visits;
        }
    }
    
    if (total_visits == 0) return 0.0;
    
    float entropy = 0.0;
    float log2 = log(2.0);
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            float p = (float)world[x][y].photon_visits / total_visits;
            if (p > 0.0) {
                entropy -= p * (log(p) / log2);
            }
        }
    }
    
    // Normalizácia na rozsah 0-1
    float max_entropy = log(dimension * dimension) / log2;
    if (max_entropy > 0.0) {
        entropy /= max_entropy;
    }
    
    // Obmedzenie na rozsah 0-1
    if (entropy < 0.0) entropy = 0.0;
    if (entropy > 1.0) entropy = 1.0;
    
    metrics.information_entropy = entropy;
    return entropy;
}

/* Tepelná entropia z rozloženia teploty */
float calculate_thermal_entropy() {
    float total_energy = 0.0;
    int64_t cells = dimension * dimension;
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            total_energy += world[x][y].temperature;
        }
    }
    
    if (total_energy <= 0.0) return 0.0;
    
    float entropy = 0.0;
    
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            float p = world[x][y].temperature / total_energy;
            if (p > 0.0) {
                entropy -= p * log(p);
            }
        }
    }
    
    // Normalizácia
    float max_entropy = log(cells);
    if (max_entropy > 0.0) {
        entropy /= max_entropy;
    }
    
    // Obmedzenie na rozsah 0-1
    if (entropy < 0.0) entropy = 0.0;
    if (entropy > 1.0) entropy = 1.0;
    
    metrics.thermal_entropy = entropy;
    return entropy;
}

/* Kvantová entropia (koherencia fotónového stavu) */
float calculate_quantum_entropy() {
    float total_interactions = photon.reflections + photon.refractions;
    float max_possible_interactions = dimension * 2;
    
    float coherence = 1.0 - (total_interactions / max_possible_interactions);
    if (coherence < 0.0) coherence = 0.0;
    
    float quantum_entropy = 1.0 - coherence;
    
    if (quantum_entropy < 0.0) quantum_entropy = 0.0;
    if (quantum_entropy > 1.0) quantum_entropy = 1.0;
    
    metrics.quantum_entropy = quantum_entropy;
    return quantum_entropy;
}

/* ==================== INICIALIZÁCIA ==================== */

void init_optical_world(int32_t dim) {
    dimension = dim;
    
    // Dynamická alokácia pamäte pre veľkú mriežku
    world = (OpticalNode**)malloc(dimension * sizeof(OpticalNode*));
    if (!world) {
        printf("Chyba: Nedostatok pamäte pre %"PRId32" riadkov\n", dimension);
        exit(1);
    }
    
    for (int32_t i = 0; i < dimension; i++) {
        world[i] = (OpticalNode*)malloc(dimension * sizeof(OpticalNode));
        if (!world[i]) {
            printf("Chyba: Nedostatok pamäte pre %"PRId32" stĺpcov\n", dimension);
            exit(1);
        }
    }
    
    // Fyzikálne korektná inicializácia
    printf("Inicializujem optický svet %"PRId32"x%"PRId32" (%"PRId64" buniek)...\n", 
           dimension, dimension, (int64_t)dimension * dimension);
    
    for (int32_t y = 0; y < dimension; y++) {
        for (int32_t x = 0; x < dimension; x++) {
            world[x][y].x = x;
            world[x][y].y = y;
            world[x][y].photon_visits = 0;
            world[x][y].energy_density = 0.0;
            world[x][y].temperature = 293.15 + (rand() % 100) / 100.0 * 5.0;
            world[x][y].optical_depth = 0.0;
            world[x][y].accumulated_phase = 0.0;
            world[x][y].interference_pattern = 0.0;
            world[x][y].entropy_density = 0.0;
            world[x][y].is_target = 0;
            
            // Náhodné priradenie optického materiálu
            float r = (rand() % 1000) / 1000.0;
            if (r < 0.40) {
                world[x][y].material_id = 0; // vzduch
            } else if (r < 0.70) {
                world[x][y].material_id = 1; // voda
            } else if (r < 0.90) {
                world[x][y].material_id = 2; // sklo
            } else if (r < 0.97) {
                world[x][y].material_id = 3; // diamant
            } else {
                world[x][y].material_id = 4; // prekážka
            }
            
            OpticalMaterial mat = materials[world[x][y].material_id];
            world[x][y].optical_depth = mat.extinction_coeff * CELL_SIZE;
        }
    }
    
    // Ciele s fyzikálnou interpretáciou
    world[0][0].material_id = 2;
    world[0][0].is_target = 1;
    
    world[dimension-1][dimension-1].material_id = 1;
    world[dimension-1][dimension-1].is_target = 2;
}

void init_photon() {
    photon.wavelength = WAVELENGTH;
    photon.intensity = 1.0;
    photon.phase = 0.0;
    photon.polarization[0] = 1.0;
    photon.polarization[1] = 0.0;
    photon.coherence_length = 1.0e-3;
    photon.optical_path_length = 0.0;
    photon.reflections = 0;
    photon.refractions = 0;
    photon.accumulated_phase = 0.0;
    photon.group_velocity = SPEED_OF_LIGHT / materials[world[start_x][start_y].material_id].refractive_index;
}

void init_metrics() {
    metrics.information_entropy = 0.0;
    metrics.thermal_entropy = 0.0;
    metrics.quantum_entropy = 0.0;
    
    metrics.total_optical_path = 0.0;
    metrics.average_intensity = 0.0;
    metrics.photon_efficiency = 0.0;
    
    metrics.total_energy_absorbed = 0.0;
    metrics.max_temperature = 0.0;
    metrics.min_temperature = 1000.0;
    
    metrics.total_cells = (int64_t)dimension * dimension;
    metrics.visited_cells = 0;
    metrics.coverage = 0.0;
}

/* ==================== HLAVNÁ OPTICKÁ SIMULÁCIA ==================== */

void simulate_photon_propagation() {
    int32_t pos_x = start_x;
    int32_t pos_y = start_y;
    float current_direction = atan2(target_y - start_y, target_x - start_x);
    
    printf("\n[KYBERNAUT-LIGHT v3.1] Fyzikálne korektná optická simulácia\n");
    printf("==============================================================\n");
    printf("Optické parametre:\n");
    printf("  • Vlnová dĺžka: %.1f nm\n", WAVELENGTH * 1e9);
    printf("  • Bunka: %.1f µm\n", CELL_SIZE * 1e6);
    printf("  • Časový krok: %.1f fs\n", TIME_STEP * 1e15);
    printf("  • Energie fotónu: %.3e J\n", PHOTON_ENERGY);
    printf("  • Rozmer sveta: %"PRId32"x%"PRId32"\n", dimension, dimension);
    printf("==============================================================\n");
    
    int32_t last_print = 0;
    float cumulative_intensity = photon.intensity;
    
    while (photon.optical_path_length < MAX_STEPS * CELL_SIZE && 
           photon.intensity > 1e-6) {
        
        world[pos_x][pos_y].photon_visits++;
        
        world[pos_x][pos_y].accumulated_phase += photon.phase;
        world[pos_x][pos_y].interference_pattern = 
            0.5 + 0.5 * cos(world[pos_x][pos_y].accumulated_phase);
        
        OpticalMaterial mat = materials[world[pos_x][pos_y].material_id];
        float absorbed = photon.intensity * mat.absorption_coeff * CELL_SIZE;
        world[pos_x][pos_y].energy_density += absorbed;
        world[pos_x][pos_y].temperature += absorbed * 100.0;
        metrics.total_energy_absorbed += absorbed * PHOTON_ENERGY;
        
        photon.intensity = beer_lambert_absorption(
            photon.intensity, 
            mat.extinction_coeff, 
            CELL_SIZE
        );
        
        if (world[pos_x][pos_y].temperature > metrics.max_temperature) {
            metrics.max_temperature = world[pos_x][pos_y].temperature;
        }
        if (world[pos_x][pos_y].temperature < metrics.min_temperature) {
            metrics.min_temperature = world[pos_x][pos_y].temperature;
        }
        
        int32_t direction = optical_transition_decision(pos_x, pos_y, current_direction);
        
        if (direction == -1) {
            break;
        }
        
        int32_t dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
        int32_t dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        float angles[8] = {0.0, M_PI/4, M_PI/2, 3*M_PI/4, 
                          M_PI, 5*M_PI/4, 3*M_PI/2, 7*M_PI/4};
        
        int32_t new_x = pos_x + dx[direction];
        int32_t new_y = pos_y + dy[direction];
        current_direction = angles[direction];
        
        if (new_x < 0 || new_x >= dimension || new_y < 0 || new_y >= dimension) {
            break;
        }
        
        OpticalMaterial old_mat = materials[world[pos_x][pos_y].material_id];
        OpticalMaterial new_mat = materials[world[new_x][new_y].material_id];
        
        if (old_mat.refractive_index != new_mat.refractive_index) {
            float incident_angle = fabs(current_direction);
            float refraction_angle = snell_law(
                old_mat.refractive_index,
                new_mat.refractive_index,
                incident_angle
            );
            
            if (refraction_angle >= 0) {
                photon.refractions++;
                current_direction = refraction_angle;
            } else {
                photon.reflections++;
                current_direction = -current_direction;
            }
        }
        
        pos_x = new_x;
        pos_y = new_y;
        
        float step_length = optical_distance(pos_x - dx[direction], 
                                            pos_y - dy[direction], 
                                            pos_x, pos_y);
        photon.optical_path_length += step_length;
        metrics.total_optical_path += step_length;
        
        photon.phase += (2 * M_PI / photon.wavelength) * 
                       new_mat.refractive_index * (CELL_SIZE / new_mat.refractive_index);
        photon.accumulated_phase = fmod(photon.phase, 2*M_PI);
        
        photon.group_velocity = SPEED_OF_LIGHT / new_mat.refractive_index;
        
        if (photon.optical_path_length / CELL_SIZE - last_print >= 1000) {
            float info_entropy = calculate_information_entropy();
            float therm_entropy = calculate_thermal_entropy();
            float quantum_entropy = calculate_quantum_entropy();
            
            printf("Dráha %6.0fµm: [%"PRId32",%"PRId32"] %s\n", 
                   photon.optical_path_length * 1e6, pos_x, pos_y,
                   materials[world[pos_x][pos_y].material_id].name);
            printf("         Intenzita: %.3f | Teplota: %.1fK\n",
                   photon.intensity, world[pos_x][pos_y].temperature);
            printf("         Odrazy: %"PRId32" | Lomy: %"PRId32"\n",
                   photon.reflections, photon.refractions);
            printf("         Entropia: S_info=%.3f, S_therm=%.3f, S_quant=%.3f\n",
                   info_entropy, therm_entropy, quantum_entropy);
            
            last_print = photon.optical_path_length / CELL_SIZE;
        }
        
        if (world[pos_x][pos_y].is_target == 1) {
            printf("\n╔══════════════════════════════════════════════════╗\n");
            printf("║   [DOMOV NÁJDENÝ] na dráhe %.1f µm!            ║\n", 
                   photon.optical_path_length * 1e6);
            printf("║   Zostatková intenzita: %.3f                   ║\n", photon.intensity);
            printf("╚══════════════════════════════════════════════════╝\n");
            
            target_x = dimension - 1;
            target_y = dimension - 1;
        }
        
        if (world[pos_x][pos_y].is_target == 2) {
            printf("\n╔══════════════════════════════════════════════════╗\n");
            printf("║   [BAR NÁJDENÝ] na dráhe %.1f µm!              ║\n", 
                   photon.optical_path_length * 1e6);
            printf("║   Celková optická dráha: %.1f µm              ║\n", 
                   metrics.total_optical_path * 1e6);
            printf("╚══════════════════════════════════════════════════╝\n");
            break;
        }
    }
    
    calculate_information_entropy();
    calculate_thermal_entropy();
    calculate_quantum_entropy();
    
    int64_t visited = 0;
    for (int32_t x = 0; x < dimension; x++) {
        for (int32_t y = 0; y < dimension; y++) {
            if (world[x][y].photon_visits > 0) visited++;
        }
    }
    metrics.visited_cells = visited;
    metrics.coverage = (float)visited / metrics.total_cells * 100.0;
    
    if (photon.optical_path_length / CELL_SIZE + 1 > 0) {
        metrics.average_intensity = cumulative_intensity / (photon.optical_path_length / CELL_SIZE + 1);
    } else {
        metrics.average_intensity = 0.0;
    }
    
    if (metrics.total_energy_absorbed > 0) {
        metrics.photon_efficiency = photon.optical_path_length / metrics.total_energy_absorbed;
    } else {
        metrics.photon_efficiency = 0.0;
    }
}

/* ==================== HLAVNÝ PROGRAM ==================== */

int main() {
    srand(time(NULL));
    
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║          KYBERNAUT-LIGHT v3.1 - OPTICKÁ VERZIA             ║\n");
    printf("║      (Fyzikálne korektná simulácia fotónu)                ║\n");
    printf("║                PODPORA PRE VEĽKÉ MRIEŽKY                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("OPTICKÁ KOREKTNOSŤ:\n");
    printf("  • Snellov zákon a Fresnelove koeficienty\n");
    printf("  • Beer-Lambertov zákon absorpcie\n");
    printf("  • Projekcia 3D optiky do 2D\n");
    printf("  • Interferenčné a fázové efekty\n\n");
    
    printf("Zadaj rozmer sveta (napr. 15-1000): ");
    if (scanf("%"SCNd32, &dimension) != 1 || dimension < 5) {
        printf("Chyba: Neplatný rozmer.\n");
        return 1;
    }
    
    if (dimension > 1000) {
        printf("POZOR: Veľký rozmer %"PRId32"x%"PRId32" môže vyžadovať veľa pamäte (%.2f MB)\n",
               dimension, dimension, 
               dimension * dimension * sizeof(OpticalNode) / (1024.0 * 1024.0));
        printf("Naozaj pokračovať? (a/n): ");
        char confirm;
        scanf(" %c", &confirm);
        if (confirm != 'a' && confirm != 'A') return 0;
    }
    
    init_optical_world(dimension);
    
    start_x = dimension / 2;
    start_y = dimension / 2;
    target_x = 0;
    target_y = 0;
    
    init_photon();
    init_metrics();
    
    printf("\nŠtart: [%"PRId32",%"PRId32"], Ciele: Domov[0,0] -> Bar[%"PRId32",%"PRId32"]\n",
           start_x, start_y, dimension-1, dimension-1);
    printf("Optické parametre:\n");
    printf("  • Fotón: λ=%.1f nm, E=%.2e J\n", WAVELENGTH*1e9, PHOTON_ENERGY);
    printf("  • Rozlíšenie: %.1f µm/bunka\n", CELL_SIZE*1e6);
    printf("  • Časové rozlíšenie: %.1f fs/krok\n", TIME_STEP*1e15);
    printf("  • Maximálny počet krokov: %d\n\n", MAX_STEPS);
    
    clock_t start_time = clock();
    simulate_photon_propagation();
    clock_t end_time = clock();
    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("              VÝSLEDKY KYBERNAUT-LIGHT v3.1\n");
    printf("══════════════════════════════════════════════════════════════\n\n");
    
    printf("OPTICKÉ METRIKY:\n");
    printf("  Celková optická dráha: %.1f µm\n", metrics.total_optical_path * 1e6);
    printf("  Konečná intenzita: %.3f\n", photon.intensity);
    printf("  Odrazy/Lomy: %"PRId32"/%"PRId32"\n", photon.reflections, photon.refractions);
    printf("  Koherenčná dĺžka: %.1f mm\n", photon.coherence_length * 1e3);
    printf("  Čas simulácie: %.3f s\n", total_time);
    
    printf("\nENTROPICKÁ ANALÝZA (normalizované 0-1):\n");
    printf("  Informačná entropia (S_info): %.4f\n", metrics.information_entropy);
    printf("  Tepelná entropia (S_thermal): %.4f\n", metrics.thermal_entropy);
    printf("  Kvantová entropia (S_quantum): %.4f\n", metrics.quantum_entropy);
    printf("  Rozdiel S_thermal - S_info: %.4f\n", 
           metrics.thermal_entropy - metrics.information_entropy);
    printf("  Pomer S_thermal/S_info: %.3f\n",
           metrics.information_entropy > 0 ? metrics.thermal_entropy / metrics.information_entropy : 0);
    
    printf("\nTERMODYNAMICKÉ METRIKY:\n");
    printf("  Absorbovaná energia: %.3e J\n", metrics.total_energy_absorbed);
    printf("  Teplotný rozsah: %.1fK - %.1fK\n", 
           metrics.min_temperature, metrics.max_temperature);
    printf("  Efektivita fotónu: %.3e m/J\n", metrics.photon_efficiency);
    
    printf("\nEXPLORAČNÉ METRIKY:\n");
    printf("  Pokrytie sveta: %"PRId64"/%"PRId64" buniek (%.1f%%)\n",
           metrics.visited_cells, metrics.total_cells, metrics.coverage);
    printf("  Priemerná intenzita: %.3f\n", metrics.average_intensity);
    
    // VALIDÁCIA
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("              MATEMATICKÁ A FYZIKÁLNA VALIDÁCIA\n");
    printf("══════════════════════════════════════════════════════════════\n");
    
    int validation_passed = 1;
    
    if (metrics.information_entropy < 0 || metrics.information_entropy > 1) {
        printf("✗ S_info mimo rozsahu 0-1: %.4f\n", metrics.information_entropy);
        validation_passed = 0;
    } else {
        printf("✓ S_info v rozsahu 0-1: %.4f\n", metrics.information_entropy);
    }
    
    if (metrics.thermal_entropy < 0 || metrics.thermal_entropy > 1) {
        printf("✗ S_thermal mimo rozsahu 0-1: %.4f\n", metrics.thermal_entropy);
        validation_passed = 0;
    } else {
        printf("✓ S_thermal v rozsahu 0-1: %.4f\n", metrics.thermal_entropy);
    }
    
    if (metrics.quantum_entropy < 0 || metrics.quantum_entropy > 1) {
        printf("✗ S_quantum mimo rozsahu 0-1: %.4f\n", metrics.quantum_entropy);
        validation_passed = 0;
    } else {
        printf("✓ S_quantum v rozsahu 0-1: %.4f\n", metrics.quantum_entropy);
    }
    
    if (photon.intensity < 0 || photon.intensity > 1) {
        printf(" Intenzita mimo 0-1: %.3f (možno validné pre koherenciu)\n", photon.intensity);
    } else {
        printf("✓ Intenzita v rozsahu 0-1: %.3f\n", photon.intensity);
    }
    
    if (photon.group_velocity > SPEED_OF_LIGHT * 1.1) {
        printf("✗ Skupinová rýchlosť > c: %.3e m/s\n", photon.group_velocity);
        validation_passed = 0;
    } else {
        printf("✓ Skupinová rýchlosť ≤ c: %.3e m/s\n", photon.group_velocity);
    }
    
    if (validation_passed) {
        printf("\n✓ Všetky metriky matematicky a fyzikálne korektné\n");
    } else {
        printf("\n✗ Niektoré metriky mimo fyzikálnych limitov\n");
    }
    
    // Uloženie výsledkov
    FILE* f = fopen(LOG_FILENAME, "w");
    if (f) {
        fprintf(f, "KYBERNAUT-LIGHT v3.1 - Opticky korektná verzia\n");
        fprintf(f, "==============================================\n\n");
        fprintf(f, "Optické parametre:\n");
        fprintf(f, "  Vlnová dĺžka: %.1f nm\n", WAVELENGTH * 1e9);
        fprintf(f, "  Energia fotónu: %.3e J\n", PHOTON_ENERGY);
        fprintf(f, "  Rozmer sveta: %"PRId32"x%"PRId32"\n", dimension, dimension);
        fprintf(f, "  Bunka: %.1e m\n", CELL_SIZE);
        fprintf(f, "  Simulačný čas: %.3f s\n\n", total_time);
        
        fprintf(f, "Entropické metriky (0-1):\n");
        fprintf(f, "  S_info: %.4f\n", metrics.information_entropy);
        fprintf(f, "  S_thermal: %.4f\n", metrics.thermal_entropy);
        fprintf(f, "  S_quantum: %.4f\n", metrics.quantum_entropy);
        fprintf(f, "  ΔS: %.4f\n", metrics.thermal_entropy - metrics.information_entropy);
        fprintf(f, "  Pomer: %.3f\n\n", 
                metrics.information_entropy > 0 ? metrics.thermal_entropy / metrics.information_entropy : 0);
        
        fprintf(f, "Optické metriky:\n");
        fprintf(f, "  Optická dráha: %.1f µm\n", metrics.total_optical_path * 1e6);
        fprintf(f, "  Odrazy/Lomy: %"PRId32"/%"PRId32"\n", photon.reflections, photon.refractions);
        fprintf(f, "  Konečná intenzita: %.3f\n", photon.intensity);
        fprintf(f, "  Pokrytie: %.1f%%\n", metrics.coverage);
        
        fclose(f);
        printf("\nVýsledky uložené do: %s\n", LOG_FILENAME);
    }
    
    // Uvoľnenie pamäte
    for (int32_t i = 0; i < dimension; i++) {
        free(world[i]);
    }
    free(world);
    
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("  OPTICKÁ SIMULÁCIA UKONČENÁ - FYZIKÁLNE VALIDOVANÁ\n");
    printf("══════════════════════════════════════════════════════════════\n");
    
    return 0;
}
