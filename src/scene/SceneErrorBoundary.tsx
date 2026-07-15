import { Component, ReactNode } from 'react';
import { t, type Language } from '@/i18n';
import { useUIStore } from '@/stores/uiStore';

interface Props {
  children: ReactNode;
  language: Language;
}

interface State {
  error: Error | null;
}

/**
 * Catches render/runtime errors from the 3D <Canvas> subtree (e.g. WebGL context
 * creation failure on a client machine with no GPU acceleration) so a viewport
 * crash degrades to a readable message instead of unmounting the whole app.
 */
class SceneErrorBoundaryInner extends Component<Props, State> {
  state: State = { error: null };

  static getDerivedStateFromError(error: Error): State {
    return { error };
  }

  componentDidCatch(error: Error) {
    console.error('[SceneErrorBoundary] 3D viewport crashed:', error);
  }

  render() {
    const { error } = this.state;
    if (!error) return this.props.children;

    const lang = this.props.language;
    return (
      <div className="scene-error">
        <div className="scene-error-icon">⚠</div>
        <div className="scene-error-title">{t(lang, 'scene.error.title')}</div>
        <div className="scene-error-hint">{t(lang, 'scene.error.hint')}</div>
        <pre className="scene-error-detail">{error.message || String(error)}</pre>
      </div>
    );
  }
}

/** Functional wrapper so the class boundary re-renders when the language changes. */
export function SceneErrorBoundary({ children }: { children: ReactNode }) {
  const language = useUIStore((s) => s.language);
  return (
    <SceneErrorBoundaryInner key={language} language={language}>
      {children}
    </SceneErrorBoundaryInner>
  );
}
