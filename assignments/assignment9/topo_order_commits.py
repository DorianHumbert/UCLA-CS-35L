import zlib
import os
import sys

'''
Do a topological sort on a top level .git directory
Print out the results in a readable format
'''

# Find the top level .git directory starting at the current directory
# Get heads of all local branches, each associated with their commit hash
def get_local_branch_heads():
	git_dir = os.getcwd()
	while git_dir != '/':
		if os.path.exists('.git') == True:
			break
		os.chdir('../')
		git_dir = os.getcwd()
	if git_dir == '/':
		sys.exit('Not inside a git repository')

	branch_location = os.path.join(git_dir, '.git/refs/heads/')
	os.chdir(branch_location)

	local_branches = {}
	for root, dirs, files in os.walk('.'):
		for name in files:
			branch_file = os.path.join(root,name)
			branch_hash = open(branch_file, 'r').read().strip()
			if branch_hash not in local_branches:
				local_branches[branch_hash] = set()
			local_branches[branch_hash].add(branch_file[2:])

	os.chdir(git_dir)
	return git_dir, local_branches

# Credits: 35L TA(s)
# Holds the commit hash, parent(s), and children of a commit object
class CommitNode:
	def __init__(self, commit_hash):
		"""                                                                                                                     
		:type commit_hash: str                                                                                                  
		"""
		self.commit_hash = commit_hash
		self.parents = set()
		self.children = set()

# Retrieve parent commit(s) from the .git/objects folder based on child commit
def get_parent_commits(git_dir, child_commit_hash):
	obj_dir = os.path.join(git_dir, '.git/objects/')
	os.chdir(obj_dir)

	parent_commits = set() # hold parent commits of passed in commit

	dir_name = child_commit_hash[0:2] # name of commit directory
	sub_folder_name = child_commit_hash[2:] # name of commit object

	filename = os.path.join(dir_name, sub_folder_name) # location of commit obj
	compressed_contents = open(filename, 'rb').read()
	decompressed_contents = zlib.decompress(compressed_contents).decode()
	decompressed_contents = decompressed_contents.split('\n')
	for decompressed_line in decompressed_contents:
		index = decompressed_line.find('parent')
		if index != -1:
			par_comm = decompressed_line[index+6:].strip()
			parent_commits.add(par_comm)

	os.chdir(git_dir)
	return parent_commits

# Use Depth First Search to build a graph of all commits
# DFS: take leaf nodes and travel down the branch, adding commits along the way
# All commits stored in commit_nodes, containing children, parents, and hash
# Root hashes stores all leaf nodes (no parents)
def build_commit_graph(git_dir, local_branch_heads):
	commit_nodes = {} # dictionary mapping commit hash to commit node
	root_hashes = set() # contains the hashes of root nodes
	visited = set() # contains all visited nodes in the commit graph

	stack = [] # stack of all branch head commit hashes
	for key, item in local_branch_heads.items():
		stack.append(key)
	while stack:
		commit_hash = stack.pop()
		# if visited, then the CommitNode object has been created
		# and parent commits have been added
		if commit_hash in visited:
			continue
		visited.add(commit_hash)
		if commit_hash not in commit_nodes:
			new_commit_node = CommitNode(commit_hash)
			commit_nodes[commit_hash] = new_commit_node
		commit_node = commit_nodes[commit_hash]
		commit_node.parents = get_parent_commits(git_dir, commit_hash)
		if not commit_node.parents:
			root_hashes.add(commit_hash)
		for p_hash in commit_node.parents:
			# add parent hash to stack of hashes to be processed
			if p_hash not in visited:
				stack.append(p_hash)
			# add parent hash to dict of CommitNode
			if p_hash not in commit_nodes:
				new_commit_node = CommitNode(p_hash)
				commit_nodes[p_hash] = new_commit_node
			# add children to the parent CommitNode
			commit_nodes[p_hash].children.add(commit_hash)

	return commit_nodes, root_hashes

# Topological sort on all commits using Depth First Search
# DFS: completely process a single branch at a time
# Ordered from least to greatest (descendant commits less than ancestral)
def get_topo_ordered_commits(commit_nodes, root_hashes):
	order = [] # ordered commits
	visited = set() # all visited commits
	temp_stack = [] # holds commit hashes until reaching a non-child commit

	stack = sorted(root_hashes) # create deterministic ordering of the result
	while stack:
		vx = stack.pop()
		if vx in visited:
			continue
		visited.add(vx)
		# if current commit is not a child of previously processed commit
		# transfer stack to order in reverse order
		while temp_stack and vx not in commit_nodes[temp_stack[-1]].children:
			d = temp_stack.pop()
			order.append(d)
		temp_stack.append(vx)
		for child in sorted(commit_nodes[vx].children):
			if child in visited:
				continue
			stack.append(child) # append child to be processed next
	while temp_stack: # transfer remaining commits
		d = temp_stack.pop()
		order.append(d)
	return order

# Credits: 35L TA(s)
# Print the topologically ordered commits with branch names
# Print with sticky ends as a visual aid for gluing separate fragments together
def print_topo_ordered_commits_with_branch_names(commit_nodes, topo_ordered_commits, head_to_branches):
	jumped = False
	for i in range(len(topo_ordered_commits)):
		commit_hash = topo_ordered_commits[i]
		if jumped:
			jumped = False
			sticky_hash = ' '.join(commit_nodes[commit_hash].children)
			print(f'={sticky_hash}')
		branches = sorted(head_to_branches[commit_hash]) if commit_hash in head_to_branches else []
		print(commit_hash + (' ' + ' '.join(branches) if branches else ''))
		if i+1 < len(topo_ordered_commits) and topo_ordered_commits[i+1] not in commit_nodes[commit_hash].parents:
			jumped = True
			sticky_hash = ' '.join(commit_nodes[commit_hash].parents)
			print(f'{sticky_hash}=\n')

# Do topological sort
# Print results
def topo_order_commits():
	git_dir, local_branch_heads = get_local_branch_heads()
	commit_nodes, root_hashes = build_commit_graph(git_dir, local_branch_heads)
	topo_ordered_commits = get_topo_ordered_commits(commit_nodes, root_hashes)
	print_topo_ordered_commits_with_branch_names(commit_nodes, topo_ordered_commits, local_branch_heads)
	
# Print results between procedures to debug
def test():
	git_dir, local_branch_heads = get_local_branch_heads()
	print (git_dir)
	for key, item in local_branch_heads.items():
		print (key, "\t", str(item))
	commit_nodes, root_hashes = build_commit_graph(git_dir, local_branch_heads)
	for key, item in commit_nodes.items():
		print (key)
	topo_ordered_commits = get_topo_ordered_commits(commit_nodes, root_hashes)
	print_topo_ordered_commits_with_branch_names(commit_nodes, topo_ordered_commits, local_branch_heads)

if __name__ == '__main__':
	topo_order_commits()