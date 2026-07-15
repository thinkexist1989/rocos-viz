# Syntax Error Report - rocos-web

## Summary
- Total issues found: 3
- Issues fixed: 3
- Issues remaining: 0

## TypeScript Compiler Output

### Before fixes
```
src/components/control/EnableButton.tsx(3,10): error TS2300: Duplicate identifier 'PoweroffOutlined'.
src/components/control/EnableButton.tsx(3,28): error TS2300: Duplicate identifier 'PoweroffOutlined'.
src/scene/TrajectoryLine.tsx(38,11): error TS2322: Type 'RefObject<Line<BufferGeometry<NormalBufferAttributes>, Material | Material[]>>' is not assignable to type 'LegacyRef<SVGLineElement> | undefined'.
```

### After fixes
```
(no output - all errors resolved)
```

## Issues Found and Fixed

### 1. src/components/control/EnableButton.tsx - Duplicate import of PoweroffOutlined
- **Severity**: Error (TS2300)
- **Description**: Line 3 imported `PoweroffOutlined` twice from `@ant-design/icons`, causing a duplicate identifier error.
- **Fix**: Removed the duplicate import, keeping only one `PoweroffOutlined` import.
- **Lines affected**: line 3

### 2. src/scene/TrajectoryLine.tsx - Type error with `<line>` JSX element and ref
- **Severity**: Error (TS2322)
- **Description**: The `<line>` JSX element in `@react-three/fiber` resolves to the SVG `<line>` element type in the TypeScript JSX namespace, causing a type mismatch when passing a `RefObject<THREE.Line>` as a ref. Additionally, the `geometry` prop was not recognized on the SVG line type. The component also used `useMemo` as a side-effect hook (for updating geometry positions), which is an anti-pattern.
- **Fix**: Refactored the component to use `<primitive>` to render the `THREE.Line` object directly, which is the correct approach for non-standard Three.js objects in R3F. The geometry is created with a default material inside the primitive. The side-effect geometry update is now properly handled in a `useFrame` callback inside a helper component. Removed unused `useRef` and `useEffect` imports from the main component.
- **Lines affected**: lines 1-42 (full rewrite of the component logic)

### 3. src/scene/AxesIndicator.tsx - Unused imports
- **Severity**: Warning (unused imports, not a compiler error due to `noUnusedLocals: false`)
- **Description**: Lines 1-2 imported `useRef` from React and `useFrame`, `useThree` from `@react-three/fiber`, but none of these were used in the component.
- **Fix**: Removed the unused imports, keeping only the `@react-three/drei` import that is actually used.
- **Lines affected**: lines 1-2

## Remaining Issues (if any)
None. All TypeScript compiler errors have been resolved.

## Build Verification

### tsc --noEmit
Result: **PASS** - No errors.

### npm run build
Result: **PASS** - Build completed successfully in 9.54s. Output:
- dist/index.html (0.47 kB)
- dist/assets/index-ClEG9Osf.css (3.23 kB)
- dist/assets/index-D5cgHeQ8.js (1,595.90 kB)

Note: Vite warns about chunk size exceeding 500 kB, suggesting code-splitting. This is not an error.
