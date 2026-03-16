# INTERCEPTOR MISSILE GUIDE : FLOW & AERODYNAMICS

---

## QUICK NOTE ON COORDINATES

**h = Z = Height/Altitude**

In all equations below:
- `z` = height above ground (meters)
- Ground level: `z = 0`
- Going up: `z` increases (positive)
- Gravity pulls **down**: `-g` in z-direction

---

## PHASE 1: PRE-LAUNCH PREDICTION

### Step 1: Target Trajectory Prediction

Given target position `(xt0, yt0, zt0)` and velocity vector `(vxt, vyt, vzt)`:

```
Target position at any future time t:
    xt(t) = xt0 + vxt * t
    yt(t) = yt0 + vyt * t
    zt(t) = zt0 + vzt * t - (1/2) * g * t²
```

The `-½gt²` term is gravity pulling the target down over time.

---

### Step 2: Intercept Equation

Your interceptor launches from `(0,0,0)` with constant speed `Vi`:

```
Distance interceptor travels in time t:  Vi * t

This must equal the distance to where target will be at time t:

(Vi * t)² = [xt(t)]² + [yt(t)]² + [zt(t)]²
```

Substitute the target equations:

```
(Vi * t)² = (xt0 + vxt*t)² + (yt0 + vyt*t)² + (zt0 + vzt*t - ½gt²)²
```

This expands to a quadratic equation in t² (actually a quartic, but solvable).

---

### Step 3: Solving for Intercept Time

The equation becomes:

```
A * t⁴ + B * t³ + C * t² + D * t + E = 0
```

Where the coefficients come from expanding the squared terms.

Realistically, solve numerically by trying values of t from 0 to max range:

```
For t = 0 to 100 seconds (step 0.1s):
    Calculate target position at t
    Calculate distance from launch to that point
    If (distance / Vi) <= t:
        INTERCEPT POSSIBLE at this t
        Record as candidate
```

Choose the earliest feasible intercept time.

---

### Step 4: Launch Angles

Once you have intercept time `t_i`:

```
Intercept point:
    Xi = xt0 + vxt * t_i
    Yi = yt0 + vyt * t_i
    Zi = zt0 + vzt * t_i - ½ * g * t_i²

Launch angles:
    Azimuth (horizontal direction) = atan2(Yi, Xi)
    Elevation (vertical angle) = asin(Zi / (Vi * t_i))
```

The elevation formula assumes your interceptor flies a straight line - in reality, gravity curves this path.

**IMPORTANT:** This pre-launch prediction assumes:
- Constant interceptor velocity (Vi)
- No atmosphere
- Perfect vacuum trajectory

The reality after launch is far more complex.

---

## LAYER 1: ATMOSPHERIC MODEL (Pressure, Density, Temperature)

Before any other calculations, you need the state of the atmosphere at the missile's current altitude:

```
ALTITUDE-BASED CALCULATIONS:

Temperature Lapse:
    T(h) = T0 - L * h
    where:
    T0 = 288.15K (sea level)
    L = 0.0065 K/m (lapse rate)

Pressure:
    P(h) = P0 * (T(h)/T0)^(g*M/(R*L))
    where:
    P0 = 101325 Pa
    M = 0.0289644 kg/mol (molar mass of air)
    R = 8.31432 J/(mol·K)
    g = 9.81 m/s²

Density:
    ρ(h) = P(h) * M / (R * T(h))

Speed of Sound:
    a(h) = sqrt(γ * R * T(h) / M)
    where γ = 1.4 (ratio of specific heats)

MACH NUMBER:
    Mach = V_missile / a(h)
```

**Why this matters:** Everything aerodynamic depends on density and Mach. Drag, lift, control effectiveness all scale with air density.

---

## PHASE 2: BOOST PHASE (Motor Burning)

**Duration:** First 2-10 seconds of flight  
**What happens:** Missile accelerates rapidly, mass decreases

### Thrust Calculation

```
Thrust at altitude:
    F_thrust(h) = F_sea_level + (P0 - P(h)) * A_exit
    
    where:
    P0 = sea level pressure (101325 Pa)
    P(h) = pressure at current altitude
    A_exit = nozzle exit area (m²)
```

**Why:** Rockets over-expand at low altitude, under-expand at high altitude. Performance peaks in vacuum.

### Mass Change

```
m(t) = m0 - ṁ * t

where:
    m0 = initial mass (kg)
    ṁ = mass flow rate (kg/s) = F_thrust / (Isp * g0)
    Isp = specific impulse (seconds)
```

### Forces During Boost

```
Vertical:
    m * az = F_thrust * sin(θ) - ½ * ρ * V² * Cd * A_ref - m * g

Horizontal:  
    m * ax = F_thrust * cos(θ) - ½ * ρ * V² * Cd * A_ref

where:
    θ = pitch angle from horizontal
    ρ = air density at current altitude
    Cd = drag coefficient (function of Mach)
    A_ref = reference area (m²)
```

### Boost Phase End Condition

```
Boost ends when: t >= burn_time OR fuel depleted
```

At burnout:
- Mass = m_burnout (lighter)
- Velocity = maximum for this stage
- Position = start of coast phase

---

## LAYER 2: AERODYNAMIC FORCES (The Complete Model)

This is where your interceptor's path diverges from the ideal vacuum trajectory:

```
DRAG FORCE (Opposes motion):
    F_drag = ½ * ρ * V² * Cd * A_ref
    
    Cd = f(Mach, angle of attack, configuration)
    - Subsonic (Mach < 0.8): Cd relatively constant
    - Transonic (0.8-1.2): Cd DRAMATICALLY increases (drag divergence)
    - Supersonic (1.2-5): Cd decreases with Mach
    - Hypersonic (>5): Cd stabilizes, heating dominates

LIFT FORCE (For maneuvering):
    F_lift = ½ * ρ * V² * Cl * A_ref
    
    Cl = f(angle of attack, Mach)
    - Used to change direction
    - Limited by stall angle

NORMAL FORCE (On control surfaces):
    F_control = ½ * ρ * V² * Cn * A_surface
    
    Cn = f(deflection angle, Mach)
    - This is how fins steer the missile
```

---

## PHASE 3: COAST PHASE (Ballistic Flight)

**Duration:** Most of the flight (seconds to minutes)  
**What happens:** No thrust, just drag and gravity

### Equations of Motion (Ballistic)

With no thrust, the forces simplify:

```
Horizontal:
    d²x/dt² = - (½ * ρ * V² * Cd * A_ref / m) * (vx / V)

Vertical:
    d²z/dt² = -g - (½ * ρ * V² * Cd * A_ref / m) * (vz / V)

where:
    V = sqrt(vx² + vz²) (total velocity)
    The (vx/V) and (vz/V) terms split drag into components
```

### Atmospheric Effects on Coast

The drag continuously reduces velocity:

```
Energy loss rate = ∫ F_drag * V dt

This means:
    - Apogee (maximum height) is lower than vacuum prediction
    - Range is shorter
    - Impact point shifts
```

### Vacuum vs Real Trajectory

```
In vacuum (no drag):
    z(t) = z0 + vz0 * t - ½gt²
    Symmetrical parabola

With atmosphere:
    - Ascent: Drag reduces upward velocity
    - Descent: Terminal velocity limit reached
    - Asymmetric path
```

---

## LAYER 3: EQUATIONS OF MOTION (Full 3D with Aerodynamics)

Your simple F = ma becomes:

```
FORCE BALANCE (3 axes):

X-direction:
    m * ax = F_thrust_x - F_drag * cos(γ) - m * g * sin(θ)

Y-direction:
    m * ay = F_control_y + F_lift * sin(φ)

Z-direction:
    m * az = F_thrust_z - F_drag * sin(γ) - m * g * cos(θ) + F_lift * cos(φ)

where:
    γ = flight path angle
    θ = pitch angle
    φ = bank angle
```

This is a coupled system - change one thing and everything adjusts.

---

## PHASE 4: TERMINAL GUIDANCE (Active Homing)

**Duration:** Last 5-30 seconds of flight  
**What happens:** Seeker locks on, missile maneuvers to hit

### Line of Sight (LOS) Calculations

```
LOS vector from missile to target:
    r = [xt - xm,  yt - ym,  zt - zm]

LOS rate (how fast the direction changes):
    λ̇ = |r × v_rel| / |r|²
    
    where:
    v_rel = target velocity - missile velocity
    × = cross product
```

### Proportional Navigation (The Core Guidance Law)

The fundamental interceptor math:

```
Commanded acceleration = N * Vc * λ̇

where:
    N = navigation constant (usually 3-5)
    Vc = closing velocity = -d|r|/dt
    λ̇ = line-of-sight rate
```

**Intuition:** If the target is moving across your view (λ̇ ≠ 0), you need to turn to keep it centered. The faster it moves across, the harder you turn.

### Augmented Proportional Navigation

For maneuvering targets:

```
a_cmd = N * Vc * λ̇ + (N/2) * a_target_perp

where:
    a_target_perp = target acceleration perpendicular to LOS
```

This anticipates where the target will be, not just where it is.

### Zero-Effort Miss (ZEM)

The predicted miss distance if you make no further maneuvers:

```
ZEM = |r + v_rel * t_go|   (simplified)

where:
    t_go = time to intercept
```

Guidance tries to drive ZEM to zero.

---

##  LAYER 4: GUIDANCE OVERRIDES (Real-time Corrections)

The missile continuously compares ACTUAL position vs PREDICTED:

```
ERROR CORRECTION:

Position error:
    Δr = r_actual - r_predicted

Velocity error:
    Δv = v_actual - v_predicted

Commanded acceleration (Proportional Navigation with feedback):
    a_cmd = N * Vc * λ̇ + Kp * Δr + Kd * Δv
    
    where:
    N = navigation gain (3-5 typical)
    Vc = closing velocity
    λ̇ = line-of-sight rate
    Kp, Kd = feedback gains

This acceleration must be ACHIEVABLE given:
    - Max g-limit (structural)
    - Max angle of attack (stall)
    - Control surface deflection limits
    - Available thrust
```

---

## PHASE 5: AEROTHERMAL EFFECTS (Heating)

**Critical for:** High-speed interceptors (Mach 3+)

### Stagnation Temperature

At the missile's nose and leading edges:

```
T0 = T * (1 + ((γ-1)/2) * M²)

where:
    T = ambient temperature (K)
    γ = ratio of specific heats (1.4 for air)
    M = Mach number
```

**Example:** At 10km altitude (T = 223K, M = 3):
```
T0 = 223 * (1 + 0.2 * 9) = 223 * 2.8 = 624K (351°C)
```

### Heat Flux

```
q = ρ * V³ * Ch

where:
    Ch = heat transfer coefficient (depends on Reynolds number)
```

**Consequence:** Double the velocity → 8× the heating.

### Material Limits

| Material | Max Temp | Used For |
|----------|----------|----------|
| Aluminum | 150°C | Subsonic missiles |
| Titanium | 400°C | Supersonic missiles |
| Steel | 600°C | High-speed components |
| Composites | 200-300°C | Lightweight structures |
| Ablatives | 1000-3000°C | Hypersonic (burns away) |

If predicted temperature exceeds material limits → structural failure.

---

## PHASE 6: CONTROL SYSTEM (Turning the Missile)

### Angle of Attack (α)

```
α = angle between missile body and velocity vector

Lift force = ½ * ρ * V² * Cl(α) * A_ref
```

Cl increases with α until stall (typically 15-25°).

### Control Surface Deflection

```
Fin normal force = ½ * ρ * V² * Cn(δ) * A_fin

where:
    δ = fin deflection angle
    Cn = normal force coefficient
```

### Moment Balance

```
I * d²θ/dt² = M_aero + M_control

where:
    I = moment of inertia
    θ = pitch angle
    M_aero = aerodynamic moment (from body)
    M_control = control moment (from fins)
```

### Command Limits

```
Maximum acceleration = min( g_limit, 
                            q * Cl_max * A_ref / m,
                            δ_max * q * Cn * A_fin * lever_arm / I )
```

The missile can only turn as hard as its weakest limit allows.

---

## PHASE 7: GUIDANCE UPDATE LOOP (100 Hz Cycle)

Every guidance cycle (50-100 Hz):

```
1. SENSORS READ:
   - Current position (GPS/INS)
   - Current velocity
   - Target position (seeker)
   - Body rates (gyros)

2. ATMOSPHERIC MODEL:
   - Lookup density at current altitude
   - Calculate Mach number
   - Update Cd, Cl coefficients

3. CALCULATE RELATIVE GEOMETRY:
   r = target_pos - missile_pos
   v_rel = target_vel - missile_vel
   t_go = |r| / |v_rel| (if closing)

4. COMPUTE LOS RATE:
   λ̇ = |r × v_rel| / |r|²

5. GUIDANCE COMMAND:
   a_cmd = N * |v_rel| * λ̇

6. CHECK FEASIBILITY:
   if a_cmd > max_accel:
       a_cmd = max_accel
       (Saturation warning)

7. CONVERT TO FIN COMMANDS:
   δ_req = f⁻¹(a_cmd) (inverse aero model)

8. CHECK FIN LIMITS:
   if δ_req > δ_max:
       δ_req = δ_max

9. SEND TO ACTUATORS

10. AERODYNAMICS RESPOND:
    - New forces generated
    - Trajectory changes
    - Heating computed

11. REPEAT until impact
```

---

## PRACTICAL EXAMPLE: ONE TIME STEP

Let's trace ONE time step in your simulator with full physics:

```
Missile at: 10,000m altitude, Mach 2.5, 30° angle of attack

1. Atmosphere at 10km:
   T = 223.15K (-50°C)
   P = 26,500 Pa (26% of sea level)
   ρ = 0.41 kg/m³
   Speed of sound = 300 m/s

2. Drag calculation:
   Cd at Mach 2.5 = 0.35
   Dynamic pressure q = ½ * 0.41 * (750)² = 115,300 Pa
   Drag force = 115,300 * 0.35 * 0.1 = 4,035 N

3. Thrust at altitude:
   Sea level thrust = 20,000 N
   Pressure ratio boost = +5,000 N
   Actual thrust = 25,000 N

4. Net acceleration:
   Mass = 200 kg (after fuel burn)
   a_net = (25,000 - 4,035)/200 = 104.8 m/s² (~10.7g)

5. Guidance command:
   Wants 15g turn (147 m/s²)
   But limited by:
   - Stall angle (max 25° AOA - currently at 30°)
   - Structural limit (12g max)
   
   So commands max 12g (117.6 m/s²)

6. Control deflection:
   12g requires 15° fin deflection
   15° at Mach 2.5 creates 8,000 N normal force
   This rotates missile to new orientation

7. Heating at leading edge:
   T_stag = 223 * (1 + 0.2 * 6.25) = 501K (228°C)
   Within titanium limits (400°C) - OK
```

This is what happens in 0.01 seconds of real flight.

---

##  SUMMARY TABLE - ALL PHASES

| Phase | Duration | Key Math | Purpose |
|-------|----------|----------|---------|
| Pre-Launch | Before launch | Quadratic solving | Find intercept point |
| Boost | 2-10s | Thrust = f(h), m(t) | Get to speed |
| Coast | Most of flight | Ballistic + drag | Fly to target area |
| Terminal | Last 5-30s | PN: a = N·Vc·λ̇ | Hit the target |
| Heating | Continuous | T0 = T(1+0.2M²) | Don't melt |
| Control | Continuous | I·θ̈ = M_aero + M_control | Steer the missile |

---

##  PUTTING IT ALL TOGETHER: Your Code vs Real System

Your `kinetic.c` has:
- Basic position/velocity tracking
- Quaternion orientation
- Simple atmospheric pressure

To make it a REAL interceptor simulator, you need:

| Your Code | Missing |
|-----------|---------|
| Constant acceleration | Thrust curve + mass loss |
| No drag | Mach-dependent Cd |
| No lift | Control surface model |
| No heating | Thermal limits |
| Perfect sensors | Noise + errors |
| No actuator limits | Slew rate + deflection limits |

---

## BOTTOM LINE

The interceptor problem flows as:

1. **PRE-LAUNCH:** Solve |Vi|·t = |r_target(t)| → find if/when you can hit
2. **BOOST:** Apply F = ma with thrust, drag, mass loss
3. **COAST:** Integrate d²r/dt² = -g - (drag/m) until near target
4. **TERMINAL:** Command a = N·Vc·λ̇ to steer onto target
5. **IMPACT:** Target destroyed

The pre-launch solution tells you IF you can hit the target. The aerodynamics and guidance tell you HOW to actually fly there and whether the missile survives the flight.

---

## REMEMBER

**h = Z = Height/Altitude** (positive upward)

In all equations:
- z = height above ground (meters)
- Ground level: z = 0
- Going up: z increases (positive)
- Gravity pulls down: -g in z-direction
