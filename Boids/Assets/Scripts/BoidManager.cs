using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Random = UnityEngine.Random;

public class BoidManager : MonoBehaviour
{
    [SerializeField] private GameObject boidPrefab;
    [SerializeField] private int count;

    [SerializeField] private float separationFactor = 4;
    [SerializeField] private float alignmentFactor = 1;
    [SerializeField] private float cohesionFactor = 0.5f;
    [SerializeField] private float obstacleAvoidanceFactor = 2;
    private List<Boid> boids;
    void Start()
    {
        boids = new List<Boid>();
        for (int i = 0; i < count; i++)
        {
            GameObject newBoid = Instantiate(boidPrefab, Random.insideUnitCircle, Quaternion.identity);
            Boid boid = newBoid.GetComponent<Boid>();
            boids.Add(boid);
            boid.Init(boids);
            boid.separationFactor = separationFactor;
            boid.alignmentFactor = alignmentFactor;
            boid.cohesionFactor = cohesionFactor;
            boid.obstacleAvoidanceFactor = obstacleAvoidanceFactor;
        }
    }

    private void Update()
    {
        foreach (Boid boid in boids)
        {
            boid.separationFactor = separationFactor;
            boid.alignmentFactor = alignmentFactor;
            boid.cohesionFactor = cohesionFactor;
            boid.obstacleAvoidanceFactor = obstacleAvoidanceFactor;
        }
    }
}
