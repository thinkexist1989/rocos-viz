import { Button } from 'antd';
import { useUIStore } from '@/stores/uiStore';

export function DisplayToggles() {
  const showAxes = useUIStore((s) => s.showAxes);
  const showWireframe = useUIStore((s) => s.showWireframe);
  const showGround = useUIStore((s) => s.showGround);
  const showTrajectory = useUIStore((s) => s.showTrajectory);
  const toggleAxes = useUIStore((s) => s.toggleAxes);
  const toggleWireframe = useUIStore((s) => s.toggleWireframe);
  const toggleGround = useUIStore((s) => s.toggleGround);
  const toggleTrajectory = useUIStore((s) => s.toggleTrajectory);

  return (
    <div className="display-toggle">
      <Button size="small" type={showAxes ? 'primary' : 'default'} onClick={toggleAxes}>
        Axes
      </Button>
      <Button size="small" type={showWireframe ? 'primary' : 'default'} onClick={toggleWireframe}>
        Mesh
      </Button>
      <Button size="small" type={showGround ? 'primary' : 'default'} onClick={toggleGround}>
        Ground
      </Button>
      <Button size="small" type={showTrajectory ? 'primary' : 'default'} onClick={toggleTrajectory}>
        Trajectory
      </Button>
    </div>
  );
}
