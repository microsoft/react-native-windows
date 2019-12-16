export interface IRNTesterExample {
  title: string;
  description: string;
  render: () => JSX.Element;
}

export interface IRNTesterPage {
  title: string;
  description?: string;
  displayName?: string;
  examples: IRNTesterExample[];
}
