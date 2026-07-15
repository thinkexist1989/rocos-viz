import { useRobotStateStore } from '@/stores/robotStateStore';
import { useControlStore } from '@/stores/controlStore';
import { useT } from '@/i18n/useT';
import { CartesianJogItem } from './CartesianJogItem';

/**
 * Null-space jog panel.
 *
 * For a robot with N degrees of freedom, the null space has max(0, N - 6)
 * dimensions.  Each dimension gets a pair of +/- jog buttons labelled
 * "Dim 1", "Dim 2", etc.
 */
export function NullSpacePanel() {
  const t = useT();
  const robotState = useRobotStateStore((s) => s.robotState);
  const currentFrame = useControlStore((s) => s.currentFrame);

  const dof = robotState?.joint_states?.length ?? 0;
  const nullDims = Math.max(0, dof - 6);

  if (nullDims === 0) return null;

  const dims = Array.from({ length: nullDims }, (_, i) => ({
    label: `N${i + 1}`,
    flagName: `N${i + 1}`,
    index: i,
  }));

  return (
    <div className="panel-section">
      <div className="panel-section-header">
        {t('jog.nullSpace')}
      </div>
      <div>
        {dims.map((dim) => (
          <CartesianJogItem
            key={dim.flagName}
            label={dim.label}
            value={0}
            frame={currentFrame}
            freedom={6 + dim.index}
            unit="deg"
            isPosition={false}
            customFlag={dim.flagName}
          />
        ))}
      </div>
    </div>
  );
}
