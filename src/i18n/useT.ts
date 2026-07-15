import { useCallback } from 'react';
import { useUIStore } from '@/stores/uiStore';
import { t, type TranslationKey } from '@/i18n';

/**
 * React hook returning a `t(key, vars)` bound to the current UI language.
 * Re-renders the calling component when the language toggles.
 */
export function useT() {
  const language = useUIStore((s) => s.language);
  return useCallback(
    (key: TranslationKey, vars?: Record<string, string | number>) => t(language, key, vars),
    [language],
  );
}
